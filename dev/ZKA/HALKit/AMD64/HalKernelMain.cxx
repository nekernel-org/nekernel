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

EXTERN_C Kernel::UInt8* mp_user_switch_proc;
EXTERN_C Kernel::UInt8* mp_user_switch_proc_stack_end;
EXTERN_C Kernel::VoidPtr mp_user_switch_proc_real;

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];
EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void);

/// @brief Kernel init procedure.
EXTERN_C void hal_init_platform(
	Kernel::HEL::HandoverInformationHeader* HandoverHeader)
{
	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	// get page size.
	kKernelBitMpSize = kHandoverHeader->f_BitMapSize;

	// get virtual address start (for the heap)
	kKernelVirtualStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	// get physical address start.
	kKernelPageStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_PageStart));

	STATIC CONST auto cEntriesCount = 6;

	/* GDT, mostly descriptors for user and kernel segments. */
	STATIC Kernel::HAL::Detail::ZKA_GDT_ENTRY ALIGN(0x08) cGdt[cEntriesCount] = {
		{.fLimitLow = 0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0},	  // Null entry
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x9A, .fFlags = 0xAF, .fBaseHigh = 0}, // Kernel code
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x92, .fFlags = 0xCF, .fBaseHigh = 0}, // Kernel data
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xF2, .fFlags = 0xCF, .fBaseHigh = 0}, // User data
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xFA, .fFlags = 0xAF, .fBaseHigh = 0}, // User code
	};

	// Load memory descriptors.
	Kernel::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<Kernel::UIntPtr>(cGdt);
	gdtBase.Limit = (sizeof(Kernel::HAL::Detail::ZKA_GDT_ENTRY) * cEntriesCount) - 1;

	CONST Kernel::HAL::GDTLoader cGDT;
	cGDT.Load(gdtBase);

	// Load IDT now.

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	Kernel::HAL::Register64 idtBase;
	idtBase.Base = (Kernel::UIntPtr)kInterruptVectorTable;

	CONST Kernel::HAL::IDTLoader cIDT;
	cIDT.Load(idtBase);

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	auto fs = Kernel::mm_new_class<Kernel::NeFileSystemMgr>();

	Kernel::NeFileSystemMgr::Mount(fs);

	ke_dll_entrypoint();

	Kernel::ke_stop(RUNTIME_CHECK_FAILED);
}
