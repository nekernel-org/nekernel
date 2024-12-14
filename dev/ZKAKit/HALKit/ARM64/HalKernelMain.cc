/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <Modules/FB/FB.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/Heap.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/Json.h>
#include <KernelKit/CodeMgr.h>
#include <Modules/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <Modules/FB/KWindow.h>
#include <CFKit/Property.h>

Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void gsh_dll_main(Kernel::Void);

EXTERN_C void hal_init_platform(
	Kernel::HEL::BootInfoHeader* handover_hdr)
{

	/************************************************** */
	/*     INITIALIZE AND VALIDATE HEADER.              */
	/************************************************** */

	kHandoverHeader = handover_hdr;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	/************************************** */
	/*     INITIALIZE BIT MAP.              */
	/************************************** */

	kKernelBitMpSize  = kHandoverHeader->f_BitMapSize;
	kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	/// @note do initialize the interrupts after it.

	CG::CGDrawBackground();

	Kernel::rtl_create_process(gsh_dll_main, "GSh");

	while (YES)
	{
	}
}
