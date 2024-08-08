/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/Json.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeManager.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

Kernel::Property cKernelVersion;
Kernel::Property cAutoFormatDisk;

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];

struct HEAP_ALLOC_INFO final
{
	Kernel::VoidPtr fThe;
	Kernel::Size	fTheSz;
};

struct PROCESS_BLOCK_INFO final
{
	THREAD_INFORMATION_BLOCK* fTIB;
	THREAD_INFORMATION_BLOCK* fGIB;
};

struct PROCESS_EXIT_INFO final
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

void hal_real_init(void);

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

	STATIC Kernel::UInt32 kTextOffsetY = 30;

	cg_write_text("NEWOSKRNL (C) ZKA TECHNOLOGIES.", kTextOffsetY, 10, RGB(0x00, 0x00, 0x00));
	kTextOffsetY += 10;
	cg_write_text("SMP OS (MAX 8 CORES).", kTextOffsetY, 10, RGB(0x00, 0x00, 0x00));

	hal_real_init();
}

void hal_real_init(void)
{
	// get page size.
	kKernelVirtualSize = kHandoverHeader->f_VirtualSize;

	// get virtual address start.
	kKernelVirtualStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_VirtualStart));

	// get physical address start.
	kKernelPhysicalStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_PhysicalStart));

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

	// Register the basic system calls.

	constexpr auto cTlsInterrupt		= 0x11;
	constexpr auto cTlsInstallInterrupt = 0x12;
	constexpr auto cNewInterrupt		= 0x13;
	constexpr auto cDeleteInterrupt		= 0x14;
	constexpr auto cExitInterrupt		= 0x15;
	constexpr auto cLastExitInterrupt	= 0x16;
	constexpr auto cCatalogOpen			= 0x17;
	constexpr auto cForkRead			= 0x18;
	constexpr auto cForkWrite			= 0x19;
	constexpr auto cCatalogClose		= 0x20;
	constexpr auto cCatalogRemove		= 0x21;
	constexpr auto cCatalogCreate		= 0x22;
	constexpr auto cRebootInterrupt		= 0x23;
	constexpr auto cShutdownInterrupt	= 0x24;
	constexpr auto cLPCSendMsg			= 0x25;
	constexpr auto cLPCOpenMsg			= 0x26;
	constexpr auto cLPCCloseMsg			= 0x27;
	constexpr auto cLPCSanitizeMsg		= 0x28;

	kSyscalls[cTlsInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		if (tls_check_syscall_impl(rdx) == false)
		{
			Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
		}
	};

	kSyscalls[cLPCSanitizeMsg].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ipc_sanitize_packet(reinterpret_cast<Kernel::IPC_MESSAGE_STRUCT*>(rdx));
	};

	kSyscalls[cNewInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HEAP_ALLOC_INFO* rdxInf = reinterpret_cast<HEAP_ALLOC_INFO*>(rdx);

		if (!rdxInf)
			return;

		// assign the fThe field with the pointer.
		rdxInf->fThe = Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().New(rdxInf->fTheSz);
	};

	kSyscalls[cDeleteInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HEAP_ALLOC_INFO* rdxInf = reinterpret_cast<HEAP_ALLOC_INFO*>(rdx);

		if (!rdxInf)
			return;

		// delete ptr with sz in mind.
		Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Delete(rdxInf->fThe, rdxInf->fTheSz);
	};

	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		PROCESS_BLOCK_INFO* rdxPb = reinterpret_cast<PROCESS_BLOCK_INFO*>(rdx);

		if (!rdxPb)
			return;

		// install the fTIB and fGIB.
		rt_install_tib(rdxPb->fTIB, rdxPb->fGIB);
	};

	kSyscalls[cExitInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		PROCESS_EXIT_INFO* rdxEi = reinterpret_cast<PROCESS_EXIT_INFO*>(rdx);

		if (!rdxEi)
			return;

		Kernel::kcout << "newoskrnl: " << rdxEi->fReason << "\r";
		Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Exit(rdxEi->fCode);
	};

	kSyscalls[cLastExitInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		PROCESS_EXIT_INFO* rdxEi = reinterpret_cast<PROCESS_EXIT_INFO*>(rdx);

		if (!rdxEi)
			return;

		rdxEi->fCode = Kernel::sched_get_exit_code();
	};

	kSyscalls[cRebootInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::PowerFactoryInterface pow(kHandoverHeader->f_HardwareTables.f_VendorPtr);
		pow.Reboot();
	};

	kSyscalls[cShutdownInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::PowerFactoryInterface pow(kHandoverHeader->f_HardwareTables.f_VendorPtr);
		pow.Shutdown();
	};

	kSyscalls[cTlsInterrupt].Leak().Leak()->fHooked		   = true;
	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cDeleteInterrupt].Leak().Leak()->fHooked	   = true;
	kSyscalls[cNewInterrupt].Leak().Leak()->fHooked		   = true;
	kSyscalls[cExitInterrupt].Leak().Leak()->fHooked	   = true;
	kSyscalls[cLastExitInterrupt].Leak().Leak()->fHooked   = true;
	kSyscalls[cShutdownInterrupt].Leak().Leak()->fHooked   = true;
	kSyscalls[cRebootInterrupt].Leak().Leak()->fHooked	   = true;
	kSyscalls[cLPCSanitizeMsg].Leak().Leak()->fHooked	   = true;

	Kernel::HAL::hal_system_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
