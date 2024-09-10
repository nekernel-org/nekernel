/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileMgr.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/Json.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeMgr.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

Kernel::Property cKernelVersion;
Kernel::User	 cUserSuper{Kernel::RingKind::kRingSuperUser, kSuperUser};

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
	EXTERN void mp_get_cores(Kernel::voidPtr rsdPtr) noexcept;
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

	// get page size.
	kKernelBitMpSize = kHandoverHeader->f_BitMapSize;

	// get virtual address start (for the heap)
	kKernelVirtualStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	// get physical address start.
	kKernelPhysicalStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_PhysicalStart));

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);
	else
		Kernel::HAL::mp_get_cores(nullptr);

	kcout << "Creating filesystem and such.\r";

	auto fs = new Kernel::NeFileSystemMgr();

	MUST_PASS(fs);
	MUST_PASS(fs->GetParser());

	Kernel::NeFileSystemMgr::Mount(fs);

	const auto cPassword = "ZKA_KERNEL_AUTHORITY";

	cUserSuper.TrySave(cPassword);

	ke_dll_entrypoint();

	Kernel::ke_stop(RUNTIME_CHECK_FAILED);
}
