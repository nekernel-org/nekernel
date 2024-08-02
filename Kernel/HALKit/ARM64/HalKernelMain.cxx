/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <Modules/CoreCG/CoreCG.hxx>
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

#define KERNEL_INIT(X) \
	X;                 \
	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);

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
	THREAD_INFORMATION_BLOCK* fTIB;
	THREAD_INFORMATION_BLOCK* fPIB;
};

struct PACKED ProcessExitInfo final
{
	STATIC constexpr auto cReasonLen = 512;

	Kernel::Int64 fCode;
	Kernel::Char  fReason[cReasonLen];
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
		Kernel::kcout << "Kernel: " << msg << "\r";
	};

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

		rdxEi->fCode = Kernel::sched_get_exit_code();
	};

	kSyscalls[cRebootInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ACPIFactoryInterface acpi(kHandoverHeader->f_HardwareTables.f_VendorPtr);
		acpi.Reboot();
	};

	kSyscalls[cShutdownInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		Kernel::ACPIFactoryInterface acpi(kHandoverHeader->f_HardwareTables.f_VendorPtr);
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

	KERNEL_INIT(KeMain());
}
