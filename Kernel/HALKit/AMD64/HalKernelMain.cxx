/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Modules/CoreCG/CoreCG.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/Json.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeManager.hpp>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPCEP.hxx>
#include <CFKit/Property.hpp>

#define mInitKernel(X) \
	X;                 \
	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);



Kernel::Property cKernelVersion;
Kernel::Property cAutoFormatDisk;

/// @brief This symbol is the kernel main symbol.
EXTERN_C void KeMain();

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];

struct PACKED HeapAllocInfo final
{
	Kernel::VoidPtr fThe;
	Kernel::Size	fTheSz;
};

struct PACKED ProcessBlockInfo final
{
	ThreadInformationBlock* fTIB;
	ThreadInformationBlock* fPIB;
};

struct PACKED ProcessExitInfo final
{
	STATIC constexpr auto cReasonLen = 512;

	Kernel::Int64 fCode;
	Kernel::Char  fReason[cReasonLen];
};

namespace Kernel::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsdPtr The 'RSD PTR' data structure.
	extern void hal_system_get_cores(Kernel::voidPtr rsdPtr);
} // namespace Kernel::HAL

/* GDT. */
STATIC Kernel::HAL::Detail::NewOSGDT cGdt = {
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // kernel code
	{0, 0, 0, 0x92, 0xaf, 0}, // kernel data
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // user code
	{0, 0, 0, 0x92, 0xaf, 0}, // user data
};

EXTERN_C void hal_init_platform(
	Kernel::HEL::HandoverInformationHeader* HandoverHeader)
{
	/* Setup globals. */

	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	kKernelVirtualSize	= HandoverHeader->f_VirtualSize;
	kKernelVirtualStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(HandoverHeader->f_VirtualStart) + cHeapStartOffset);

	kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

	// Load memory descriptors.

	Kernel::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<Kernel::UIntPtr>(&cGdt);
	gdtBase.Limit = sizeof(Kernel::HAL::Detail::NewOSGDT) - 1;

	CONST Kernel::HAL::GDTLoader cGDT;
	cGDT.Load(gdtBase);

	// Load IDT now.

	Kernel::HAL::Register64 idtBase;
	idtBase.Base  = (Kernel::UIntPtr)kInterruptVectorTable;
	idtBase.Limit = 0;

	CONST Kernel::HAL::IDTLoader cIDT;
	cIDT.Load(idtBase);

	// Register the basic SCI functions.

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
	constexpr auto cLPCSendMsg			 = 0x25;
	constexpr auto cLPCOpenMsg			 = 0x26;
	constexpr auto cLPCCloseMsg			 = 0x27;
	constexpr auto cLPCSanitizeMsg		 = 0x28;

	kSyscalls[cSerialAlertInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		const char* msg = (const char*)rdx;
		Kernel::kcout << "Native Log: " << msg << "\r";
	};

	kSyscalls[cTlsInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		tls_check_syscall_impl(rdx);
	};

	kSyscalls[cLPCSanitizeMsg].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ipc_sanitize_packet(reinterpret_cast<Kernel::IPCEPMessageHeader*>(rdx));
	};

	kSyscalls[cNewInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);

		if (!rdxInf)
			return;

		// assign the fThe field with the pointer.
		rdxInf->fThe = Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().New(rdxInf->fTheSz);
	};

	kSyscalls[cDeleteInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);

		if (!rdxInf)
			return;

		// delete ptr with sz in mind.
		Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Delete(rdxInf->fThe, rdxInf->fTheSz);
	};

	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		ProcessBlockInfo* rdxPb = reinterpret_cast<ProcessBlockInfo*>(rdx);

		if (!rdxPb)
			return;

		// install the fTIB and fPIB.
		rt_install_tib(rdxPb->fTIB, rdxPb->fPIB);
	};

	kSyscalls[cExitInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		ProcessExitInfo* rdxEi = reinterpret_cast<ProcessExitInfo*>(rdx);

		if (!rdxEi)
			return;

		Kernel::kcout << "newoskrnl: " << rdxEi->fReason << "\r";
		Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Exit(rdxEi->fCode);
	};

	kSyscalls[cLastExitInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		ProcessExitInfo* rdxEi = reinterpret_cast<ProcessExitInfo*>(rdx);

		if (!rdxEi)
			return;

		rdxEi->fCode = Kernel::rt_get_exit_code();
	};

	kSyscalls[cRebootInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ACPIFactoryInterface acpi(kHandoverHeader->f_HardwareTables.f_RsdPtr);
		acpi.Reboot();
	};

	kSyscalls[cShutdownInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ACPIFactoryInterface acpi(kHandoverHeader->f_HardwareTables.f_RsdPtr);
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
	kSyscalls[cRebootInterrupt].Leak().Leak()->fHooked		= true;
	kSyscalls[cLPCSanitizeMsg].Leak().Leak()->fHooked		= true;

	// newoskrnl version 1.00
	Kernel::StringView strVer(cMaxPropLen);
	strVer += "\\Properties\\KernelVersion";

	cKernelVersion.GetKey() = strVer;
    cKernelVersion.GetValue() = 100;

    Kernel::StringView strAutoMount(cMaxPropLen);
	strAutoMount += "\\Properties\\AutoMountFS?";

	cAutoFormatDisk.GetKey() = strAutoMount;
    cAutoFormatDisk.GetValue() = Yes;

	Kernel::HAL::hal_system_get_cores(kHandoverHeader->f_HardwareTables.f_RsdPtr);

	mInitKernel(KeMain());
}
