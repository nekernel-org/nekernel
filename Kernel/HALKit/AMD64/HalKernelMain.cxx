/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Modules/CoreCG/CoreCG.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/Json.hpp>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeManager.hpp>

#define KERNEL_INIT(X) X; \
	NewOS::ke_stop(RUNTIME_CHECK_BOOTSTRAP);



/// @brief This symbol is the kernel main symbol.
EXTERN_C void KeMain();

EXTERN_C NewOS::VoidPtr kInterruptVectorTable[];

struct PACKED HeapAllocInfo final
{
	NewOS::VoidPtr fThe;
	NewOS::Size	   fTheSz;
};

struct PACKED ProcessBlockInfo final
{
	ThreadInformationBlock* fTIB;
	ThreadInformationBlock* fPIB;
};

struct PACKED ProcessExitInfo final
{
	STATIC constexpr auto cReasonLen = 512;

	NewOS::Int64 fCode;
	NewOS::Char	 fReason[cReasonLen];
};

namespace NewOS::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsdPtr The 'RSD PTR' data structure.
	extern void hal_system_get_cores(NewOS::voidPtr rsdPtr);
} // namespace NewOS::HAL

/* GDT. */
STATIC NewOS::HAL::Detail::NewOSGDT cGdt = {
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // kernel code
	{0, 0, 0, 0x92, 0xaf, 0}, // kernel data
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // user code
	{0, 0, 0, 0x92, 0xaf, 0}, // user data
};

EXTERN_C void hal_init_platform(
	NewOS::HEL::HandoverInformationHeader* HandoverHeader)
{
	/* Setup globals. */

	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	kKernelVirtualSize	= HandoverHeader->f_VirtualSize;
	kKernelVirtualStart = reinterpret_cast<NewOS::VoidPtr>(
		reinterpret_cast<NewOS::UIntPtr>(HandoverHeader->f_VirtualStart) + cHeapStartOffset);

	kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

	// Load memory descriptors.

	NewOS::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<NewOS::UIntPtr>(&cGdt);
	gdtBase.Limit = sizeof(NewOS::HAL::Detail::NewOSGDT) - 1;

	CONST NewOS::HAL::GDTLoader cGDT;
	cGDT.Load(gdtBase);

	// Load IDT now.

	NewOS::HAL::Register64 idtBase;
	idtBase.Base  = (NewOS::UIntPtr)kInterruptVectorTable;
	idtBase.Limit = 0;

	CONST NewOS::HAL::IDTLoader cIDT;
	cIDT.Load(idtBase);

	// register the basic NAPI syscalls.

	constexpr auto cSerialAlertInterrupt = 0x10;
	constexpr auto cTlsInterrupt		 = 0x11;
	constexpr auto cTlsInstallInterrupt	 = 0x12;
	constexpr auto cNewInterrupt		 = 0x13;
	constexpr auto cDeleteInterrupt		 = 0x14;
	constexpr auto cExitInterrupt		 = 0x15;
	constexpr auto cLastExitInterrupt	 = 0x16;
	constexpr auto cCatalogOpen			 = 0x17;
	constexpr auto cForkRead			 = 0x18;
	constexpr auto cForkWrite			 = 0x19;
	constexpr auto cCatalogClose		 = 0x20;
	constexpr auto cCatalogRemove		 = 0x21;
	constexpr auto cCatalogCreate		 = 0x22;
	constexpr auto cRebootInterrupt		 = 0x23;
	constexpr auto cShutdownInterrupt	 = 0x24;
	constexpr auto cLPCSendMsg	 		 = 0x25;
	constexpr auto cLPCOpenMsg	 		 = 0x26;
	constexpr auto cLPCCloseMsg	 		 = 0x27;

	kSyscalls[cSerialAlertInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		const char* msg = (const char*)rdx;
		NewOS::kcout << "NAPI: " << msg << "\r";
	};

	kSyscalls[cTlsInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		tls_check_syscall_impl(rdx);
	};

	kSyscalls[cNewInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		/// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);
		
		if (!rdxInf) return;

		/// assign the fThe field with the pointer.
		rdxInf->fThe = NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().New(rdxInf->fTheSz);
	};

	kSyscalls[cDeleteInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		/// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);

		if (!rdxInf) return;

		/// delete ptr with sz in mind.
		NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Delete(rdxInf->fThe, rdxInf->fTheSz);
	};

	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		ProcessBlockInfo* rdxPb = reinterpret_cast<ProcessBlockInfo*>(rdx);
		
		if (!rdxPb) return;

		/// install the process's fTIB and fPIB.
		rt_install_tib(rdxPb->fTIB, rdxPb->fPIB);
	};

	kSyscalls[cExitInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		ProcessExitInfo* rdxEi = reinterpret_cast<ProcessExitInfo*>(rdx);
		
		if (!rdxEi) return;

		NewOS::kcout << "newoskrnl: " << rdxEi->fReason << "\r";
		NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Exit(rdxEi->fCode);
	};

	kSyscalls[cLastExitInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		ProcessExitInfo* rdxEi = reinterpret_cast<ProcessExitInfo*>(rdx);
		
		if (!rdxEi) return;

		rdxEi->fCode		   = NewOS::rt_get_exit_code();
	};

	kSyscalls[cRebootInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		NewOS::ACPIFactoryInterface acpi;
		acpi.Reboot();
	};

	kSyscalls[cShutdownInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		NewOS::ACPIFactoryInterface acpi;
		acpi.Shutdown();
	};

	kSyscalls[cSerialAlertInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cTlsInterrupt].Leak().Leak()->fHooked			= true;
	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fHooked	= true;
	kSyscalls[cDeleteInterrupt].Leak().Leak()->fHooked		= true;
	kSyscalls[cNewInterrupt].Leak().Leak()->fHooked			= true;
	kSyscalls[cExitInterrupt].Leak().Leak()->fHooked		= true;
	kSyscalls[cLastExitInterrupt].Leak().Leak()->fHooked	= true;
	kSyscalls[cShutdownInterrupt].Leak().Leak()->fHooked	= true;
	kSyscalls[cRebootInterrupt].Leak().Leak()->fHooked	= true;

	NewOS::UIAccessibilty::The().Show("RCM", NewOS::UIAccessibilty::The().Width(), NewOS::UIAccessibilty::The().Height());
	NewOS::HAL::hal_system_get_cores(kHandoverHeader->f_HardwareTables.f_RsdPtr);

	KERNEL_INIT(KeMain());
}
