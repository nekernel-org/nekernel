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
#include <NewKit/Json.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/CodeManager.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

struct HEAP_ALLOC_INFO final
{
	Kernel::VoidPtr fThe;
	Kernel::Size	fTheSz;
};

struct CREATE_THREAD_INFO final
{
	Kernel::MainKind fMain;
	Kernel::Char	 fName[kPefNameLen];
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

Kernel::Property cKernelVersion;
Kernel::User cUserSuper{Kernel::RingKind::kRingSuperUser, kSuperUser};

EXTERN Kernel::Boolean kAllocationInProgress;
EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];

Kernel::Void hal_real_init(Kernel::Void) noexcept;

EXTERN_C void hal_user_code_start(void);
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void);


/* GDT, mostly descriptors for user and kernel segments. */
STATIC Kernel::HAL::Detail::ZKA_GDT_ENTRY cGdt[9] = {
	{.fLimitLow = 0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0},		// Null entry
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x9A, .fFlags = 0xA0, .fBaseHigh = 0}, // Kernel code
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x92, .fFlags = 0xA0, .fBaseHigh = 0}, // Kernel data
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xFA, .fFlags = 0xA0, .fBaseHigh = 0}, // User code
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xF2, .fFlags = 0xA0, .fBaseHigh = 0}, // User data
	// reserve them for later.
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0}, // User data
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0}, // User data
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0}, // User data
	{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0}, // User data
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

	// Load memory descriptors.
	Kernel::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<Kernel::UIntPtr>(cGdt);
	gdtBase.Limit = (sizeof(Kernel::HAL::Detail::ZKA_GDT_ENTRY) * 9);

	CONST Kernel::HAL::GDTLoader cGDT;
	cGDT.Load(gdtBase);

	// Load IDT now.

	Kernel::HAL::Register64 idtBase;
	idtBase.Base  = (Kernel::UIntPtr)kInterruptVectorTable;
	idtBase.Limit = 0;

	CONST Kernel::HAL::IDTLoader cIDT;
	cIDT.Load(idtBase);

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::kcout << "newoskrnl.exe: Creating filesystem and such.\r";

	auto fs = new Kernel::NewFilesystemManager();

	MUST_PASS(fs);
	MUST_PASS(fs->GetParser());

	Kernel::NewFilesystemManager::Mount(fs);

	const auto cPassword = "ZKA_KERNEL_AUTHORITY";

	cUserSuper.TrySave(cPassword);

	ke_dll_entrypoint();

	Kernel::ke_stop(RUNTIME_CHECK_FAILED);
}
