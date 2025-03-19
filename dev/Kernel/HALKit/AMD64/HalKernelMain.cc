/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/AHCI.h>
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

STATIC NeOS::Void hal_init_scheduler_team()
{
	for (NeOS::SizeT i = 0U; i < NeOS::UserProcessScheduler::The().CurrentTeam().AsArray().Count(); ++i)
	{
		NeOS::UserProcessScheduler::The().CurrentTeam().AsArray()[i]		= NeOS::UserProcess();
		NeOS::UserProcessScheduler::The().CurrentTeam().AsArray()[i].Status = NeOS::ProcessStatusKind::kKilled;
	}
}

STATIC NeOS::UInt64 hal_rdtsc_fn()
{
	NeOS::UInt32 lo, hi;
	__asm__ volatile("rdtsc"
					 : "=a"(lo), "=d"(hi));

	return ((NeOS::UInt64)hi << 32) | lo;
}

STATIC NeOS::UInt64 kStart, kEnd;

/// @brief Kernel init procedure.
EXTERN_C void hal_init_platform(
	NeOS::HEL::BootInfoHeader* handover_hdr)
{
	kStart = hal_rdtsc_fn();

	kHandoverHeader = handover_hdr;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	hal_init_scheduler_team();

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
	auto dev = NeOS::sk_init_ahci_device(NO);

	NeOS::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	NeOS::HAL::Register64 idt_reg;

	idt_reg.Base = (NeOS::UIntPtr)kInterruptVectorTable;

	NeOS::HAL::IDTLoader idt_loader;

	kEnd = hal_rdtsc_fn();

	kout << "Cycles Spent Before Userland: " << NeOS::number(kEnd - kStart) << kendl;

	idt_loader.Load(idt_reg);

	while (YES)
	{
		continue;
	}
}
