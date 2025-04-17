/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include "modules/CoreGfx/CoreGfx.h"
#include <StorageKit/AHCI.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/CodeMgr.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <CFKit/Property.h>
#include <modules/CoreGfx/TextGfx.h>
#include <KernelKit/Timer.h>

#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/EFI/EFI.h>


EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];
EXTERN_C Kernel::VoidPtr mp_user_switch_proc;
EXTERN_C Kernel::Char mp_user_switch_proc_stack_begin[];

STATIC Kernel::Void hal_init_scheduler_team()
{
	for (Kernel::SizeT i = 0U; i < Kernel::UserProcessScheduler::The().CurrentTeam().AsArray().Count(); ++i)
	{
		Kernel::UserProcessScheduler::The().CurrentTeam().AsArray()[i]		  = Kernel::Process();
		Kernel::UserProcessScheduler::The().CurrentTeam().AsArray()[i].Status = Kernel::ProcessStatusKind::kKilled;
	}
}

/// @brief Kernel init procedure.
EXTERN_C Int32 hal_init_platform(
	Kernel::HEL::BootInfoHeader* handover_hdr)
{
	if (handover_hdr->f_Magic != kHandoverMagic &&
		handover_hdr->f_Version != kHandoverVersion)
	{
		return kEfiFail;
	}

	kHandoverHeader = handover_hdr;

	FB::fb_clear_video();

	(Void)(Kernel::kout << "Welcome to NeKernel.\r");

	fw_init_efi((EfiSystemTable*)handover_hdr->f_FirmwareCustomTables[1]);
	Boot::ExitBootServices(handover_hdr->f_HardwareTables.f_ImageKey, handover_hdr->f_HardwareTables.f_ImageHandle);

	hal_init_scheduler_team();

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
	STATIC Kernel::HAL::Detail::NE_GDT_ENTRY ALIGN(0x08) kGDTArray[kGDTEntriesCount] = {
		{.fLimitLow = 0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x00, .fFlags = 0x00, .fBaseHigh = 0},   // Null entry
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x9A, .fFlags = 0xAF, .fBaseHigh = 0}, // Kernel code
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0x92, .fFlags = 0xCF, .fBaseHigh = 0}, // Kernel data
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xFA, .fFlags = 0xAF, .fBaseHigh = 0}, // User code
		{.fLimitLow = 0x0, .fBaseLow = 0, .fBaseMid = 0, .fAccessByte = 0xF2, .fFlags = 0xCF, .fBaseHigh = 0}, // User data
	};

	// Load memory descriptors.
	Kernel::HAL::Register64 gdt_reg;

	gdt_reg.Base  = reinterpret_cast<Kernel::UIntPtr>(kGDTArray);
	gdt_reg.Limit = (sizeof(Kernel::HAL::Detail::NE_GDT_ENTRY) * kGDTEntriesCount) - 1;

	//! GDT will load hal_read_init after it successfully loads the segments.
	Kernel::HAL::GDTLoader gdt_loader;
	gdt_loader.Load(gdt_reg);

	return kEfiFail;
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept
{
	Kernel::NeFS::fs_init_nefs();

	Kernel::HAL::mp_init_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::HAL::Register64 idt_reg;
	idt_reg.Base = (Kernel::UIntPtr)kInterruptVectorTable;

	Kernel::HAL::IDTLoader idt_loader;

	idt_loader.Load(idt_reg);

	dbg_break_point();
}
