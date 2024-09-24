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
#include <Modules/CoreCG/WindowRenderer.hxx>

namespace Kernel::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsdPtr The 'RSD PTR' data structure.
	EXTERN void mp_get_cores(Kernel::voidPtr rsdPtr) noexcept;
} // namespace Kernel::HAL

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];
EXTERN_C Kernel::VoidPtr mp_user_switch_proc;

/// @brief Kernel init procedure.
EXTERN_C void hal_init_platform(
	Kernel::HEL::HANDOVER_INFO_HEADER* HandoverHeader)
{
	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	CG::CGDrawBackground();

	// get page size.
	kKernelBitMpSize = kHandoverHeader->f_BitMapSize;

	// get virtual address start (for the heap)
	kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

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

	//! GDT will load hal_read_init after it successfully loads the segments.
	Kernel::HAL::GDTLoader gdtLoader;
	gdtLoader.Load(gdtBase);

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	Kernel::HAL::Register64 idtBase;
	idtBase.Base = (Kernel::UIntPtr)kInterruptVectorTable;

	Kernel::HAL::IDTLoader idtLoader;
	idtLoader.Load(idtBase);

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::NeFileSystemMgr* mgr = Kernel::mm_new_class<Kernel::NeFileSystemMgr>();
	Kernel::NeFileSystemMgr::Mount(mgr);

	Kernel::HAL::mm_map_page(mp_user_switch_proc, Kernel::HAL::eFlagsUser);

	mp_do_user_switch();

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
