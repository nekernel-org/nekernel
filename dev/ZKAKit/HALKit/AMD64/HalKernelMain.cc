/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/CodeMgr.h>
#include <Modules/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <CFKit/Property.h>
#include <Modules/FB/Text.h>

EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];
EXTERN_C Kernel::VoidPtr mp_user_switch_proc;
EXTERN_C Kernel::Char mp_user_switch_proc_stack_begin[];

EXTERN_C Kernel::MainKind __CTOR_LIST__[];
EXTERN_C Kernel::MainKind __DTOR_LIST__[];

namespace Kernel
{
	EXTERN ProcessID kProcessIDCounter;
}

STATIC Kernel::Void hal_init_cxx_ctors()
{
	Kernel::kProcessIDCounter = 0UL;

	for (Kernel::SizeT index = 0UL; __CTOR_LIST__[index] != __DTOR_LIST__[0]; ++index)
	{
		Kernel::MainKind constructor_cxx = (Kernel::MainKind)__CTOR_LIST__[index];
		constructor_cxx();
	}
}

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

	hal_init_cxx_ctors();

	/************************************** */
	/*     INITIALIZE BIT MAP.              */
	/************************************** */

	kKernelBitMpSize  = kHandoverHeader->f_BitMapSize;
	kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	/************************************** */
	/*     INITIALIZE GDT AND SEGMENTS. */
	/************************************** */

	STATIC CONST auto kGDTEntriesCount = 6;

	/* GDT, mostly descriptors for user and kernel segments. */
	STATIC Kernel::HAL::Detail::ZKA_GDT_ENTRY ALIGN(0x08) kGDTArray[kGDTEntriesCount] = {
		{.fLimitLow = 0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0},	  // Null entry
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x9A, .fFlags = 0xAF, .fBaseHigh = 0}, // Kernel code
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x92, .fFlags = 0xCF, .fBaseHigh = 0}, // Kernel data
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xF2, .fFlags = 0xCF, .fBaseHigh = 0}, // User data
		{.fLimitLow = 0xFFFF, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xFA, .fFlags = 0xAF, .fBaseHigh = 0}, // User code
	};

	// Load memory descriptors.
	Kernel::HAL::RegisterGDT gdt_reg;

	gdt_reg.Base  = reinterpret_cast<Kernel::UIntPtr>(kGDTArray);
	gdt_reg.Limit = (sizeof(Kernel::HAL::Detail::ZKA_GDT_ENTRY) * kGDTEntriesCount) - 1;

	//! GDT will load hal_read_init after it successfully loads the segments.
	Kernel::HAL::GDTLoader gdt_loader;
	gdt_loader.Load(gdt_reg);

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	/* Initialize filesystem. */
	Kernel::NeFileSystemMgr::Mount(new Kernel::NeFileSystemMgr());

	const Kernel::Char process_name[] = "Kernel";

	Kernel::rtl_create_process([]() -> void {
		while (Yes)
			;
	},
							   process_name);

	/* Load interrupts and start SMP. */

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::HAL::Register64 idt_reg;
	idt_reg.Base = (Kernel::UIntPtr)kInterruptVectorTable;

	Kernel::HAL::IDTLoader idt_loader;
	idt_loader.Load(idt_reg);

	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
