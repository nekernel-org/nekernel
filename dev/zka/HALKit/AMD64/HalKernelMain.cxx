/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/HardwareThreadScheduler.hxx>
#include <KernelKit/CodeMgr.hxx>
#include <modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>
#include <modules/FB/Text.hxx>

namespace Kernel::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsp_ptr The 'RSD PTR' data structure.
	EXTERN void mp_get_cores(Kernel::voidPtr rsp_ptr) noexcept;
} // namespace Kernel::HAL

namespace Kernel
{
	EXTERN UserProcessScheduler* cProcessScheduler;
	EXTERN HardwareThreadScheduler* cHardwareThreadScheduler;
}

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];
EXTERN_C Kernel::VoidPtr mp_user_switch_proc;
EXTERN_C Kernel::Char mp_user_switch_proc_stack_begin[];

/// @brief Kernel init procedure.
EXTERN_C void hal_init_platform(
	Kernel::HEL::HANDOVER_INFO_HEADER* HandoverHeader)
{
	kHandoverHeader = HandoverHeader;

	Kernel::cProcessScheduler = nullptr;
	Kernel::cHardwareThreadScheduler = nullptr;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	/************************************** */
	/*     INITIALIZE BIT MAP.              */
	/************************************** */

	kKernelBitMpSize = kHandoverHeader->f_BitMapSize;
	kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	/************************************** */
	/*     INITIALIZE GDT AND SEGMENTS. */
	/************************************** */

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
	Kernel::HAL::RegisterGDT gdt_reg;

	gdt_reg.Base  = reinterpret_cast<Kernel::UIntPtr>(cGdt);
	gdt_reg.Limit = (sizeof(Kernel::HAL::Detail::ZKA_GDT_ENTRY) * cEntriesCount) - 1;

	//! GDT will load hal_read_init after it successfully loads the segments.
	Kernel::HAL::GDTLoader gdt_loader;
	gdt_loader.Load(gdt_reg);

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	Kernel::HAL::Register64 idt_reg;
	idt_reg.Base = (Kernel::UIntPtr)kInterruptVectorTable;

	Kernel::HAL::IDTLoader idt_loader;
	idt_loader.Load(idt_reg);

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::NeFileSystemMgr* mgr = Kernel::mm_new_class<Kernel::NeFileSystemMgr>();
	Kernel::NeFileSystemMgr::Mount(mgr);

	Kernel::UserProcessHelper::InitializeScheduler();

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
