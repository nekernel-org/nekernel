/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/Json.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeManager.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

Kernel::Property cKernelVersion;
Kernel::User cUserSuper{Kernel::RingKind::kRingSuperUser, kSuperUser};

EXTERN Kernel::Boolean kAllocationInProgress;

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
	EXTERN void mp_get_cores(Kernel::voidPtr rsdPtr);
} // namespace Kernel::HAL

Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void);

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

	hal_real_init();
}

Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	// reset kAllocationInProgress field to zero.
	kAllocationInProgress = false;

	kKernelVMHStart = kHandoverHeader->f_HeapStart;

	// get page size.
	kKernelVirtualSize = kHandoverHeader->f_VirtualSize;

	// get virtual address start (for the heap)
	kKernelVirtualStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_VirtualStart));

	// get physical address start.
	kKernelPhysicalStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_PhysicalStart));

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

	kSyscalls[cTlsInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		if (tls_check_syscall_impl(rdx) == false)
		{
			Kernel::UserProcessScheduler::The().Leak().CurrentProcess().Leak().Crash();
		}
	};

	kSyscalls[cNewInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HEAP_ALLOC_INFO* rdxInf = reinterpret_cast<HEAP_ALLOC_INFO*>(rdx);

		if (!rdxInf)
			return;

		// assign the fThe field with the pointer.
		rdxInf->fThe = Kernel::UserProcessScheduler::The().Leak().CurrentProcess().Leak().New(rdxInf->fTheSz);
	};

	kSyscalls[cDeleteInterrupt].Leak().Leak()->fProc = [](Kernel::VoidPtr rdx) -> void {
		// get HAC struct.
		HEAP_ALLOC_INFO* rdxInf = reinterpret_cast<HEAP_ALLOC_INFO*>(rdx);

		if (!rdxInf)
			return;

		// delete ptr with sz in mind.
		Kernel::UserProcessScheduler::The().Leak().CurrentProcess().Leak().Delete(rdxInf->fThe, rdxInf->fTheSz);
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

		Kernel::kcout << "newoskrnl.dll: " << rdxEi->fReason << "\r";
		Kernel::UserProcessScheduler::The().Leak().CurrentProcess().Leak().Exit(rdxEi->fCode);
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

	if (kHandoverHeader->f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);
	else
		Kernel::HAL::mp_get_cores(nullptr);

	Kernel::kcout << "newoskrnl.dll: Creating filesystem and such.\r";

	auto fs = new Kernel::NewFilesystemManager();

	MUST_PASS(fs);
	MUST_PASS(fs->GetParser());

	Kernel::NewFilesystemManager::Mount(fs);

	const auto cPassword = "ZKA_KERNEL_AUTHORITY";

	cUserSuper.TrySave(cPassword);

	ke_dll_entrypoint();

	Kernel::ke_stop(RUNTIME_CHECK_FAILED);
}
