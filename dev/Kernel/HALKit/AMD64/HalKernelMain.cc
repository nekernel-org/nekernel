/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/CodeMgr.h>
#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <CFKit/Property.h>
#include <Mod/CoreGfx/TextMgr.h>

EXTERN_C NeOS::VoidPtr kInterruptVectorTable[];
EXTERN_C NeOS::VoidPtr mp_user_switch_proc;
EXTERN_C NeOS::Char mp_user_switch_proc_stack_begin[];

EXTERN_C NeOS::rtl_ctor_kind __CTOR_LIST__[];
EXTERN_C NeOS::VoidPtr __DTOR_LIST__;

EXTERN_C NeOS::Void rtl_kernel_main(NeOS::SizeT argc, char** argv, char** envp, NeOS::SizeT envp_len);

STATIC NeOS::Void hal_init_cxx_ctors()
{
	for (NeOS::SizeT i = 0U; i < NeOS::UserProcessScheduler::The().CurrentTeam().AsArray().Count(); ++i)
	{
		NeOS::UserProcessScheduler::The().CurrentTeam().AsArray()[i]		= NeOS::UserProcess();
		NeOS::UserProcessScheduler::The().CurrentTeam().AsArray()[i].Status = NeOS::ProcessStatusKind::kKilled;
	}

	NeOS::UserProcessScheduler::The().CurrentTeam().mProcessCount = 0UL;

	for (NeOS::SizeT index = 0UL; __CTOR_LIST__[index] != __DTOR_LIST__; ++index)
	{
		NeOS::rtl_ctor_kind constructor_cxx = (NeOS::rtl_ctor_kind)__CTOR_LIST__[index];
		constructor_cxx();
	}
}

/// @brief Kernel init procedure.
EXTERN_C void hal_init_platform(
	NeOS::HEL::BootInfoHeader* handover_hdr)
{
	kHandoverHeader = handover_hdr;

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
	kKernelBitMpStart = reinterpret_cast<NeOS::VoidPtr>(
		reinterpret_cast<NeOS::UIntPtr>(kHandoverHeader->f_BitMapStart));

	/************************************** */
	/*     INITIALIZE GDT AND SEGMENTS. */
	/************************************** */

	STATIC CONST auto kGDTEntriesCount = 6;

	/* GDT, mostly descriptors for user and kernel segments. */
	STATIC NeOS::HAL::Detail::NE_GDT_ENTRY ALIGN(0x08) kGDTArray[kGDTEntriesCount] = {
		{.fLimitLow = 0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0},   // Null entry
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x9A, .fFlags = 0xAF, .fBaseHigh = 0}, // Kernel code
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x92, .fFlags = 0xCF, .fBaseHigh = 0}, // Kernel data
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xFA, .fFlags = 0xAF, .fBaseHigh = 0}, // User code
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xF2, .fFlags = 0xCF, .fBaseHigh = 0}, // User data
	};

	// Load memory descriptors.
	NeOS::HAL::RegisterGDT gdt_reg;

	gdt_reg.Base  = reinterpret_cast<NeOS::UIntPtr>(kGDTArray);
	gdt_reg.Limit = (sizeof(NeOS::HAL::Detail::NE_GDT_ENTRY) * kGDTEntriesCount) - 1;

	FB::fb_clear_video();

	//! GDT will load hal_read_init after it successfully loads the segments.
	NeOS::HAL::GDTLoader gdt_loader;
	gdt_loader.Load(gdt_reg);

	NeOS::ke_panic(RUNTIME_CHECK_BOOTSTRAP);
}

EXTERN_C NeOS::Void hal_real_init(NeOS::Void) noexcept
{
	rtl_kernel_main(0, nullptr, nullptr, 0);

	NeOS::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	NeOS::HAL::Register64 idt_reg;

	idt_reg.Base = (NeOS::UIntPtr)kInterruptVectorTable;

	NeOS::HAL::IDTLoader idt_loader;

	idt_loader.Load(idt_reg);

	while (YES)
	{
		continue;
	}
}
