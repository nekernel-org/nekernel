/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <Mod/GfxMgr/FBMgr.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/Heap.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/Json.h>
#include <KernelKit/CodeMgr.h>
#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <CFKit/Property.h>

Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void rtl_kernel_main(Kernel::SizeT argc, char** argv, char** envp, Kernel::SizeT envp_len);

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

	auto str_proc = Kernel::rt_alloc_string("System");
	Kernel::rtl_create_process(rtl_kernel_main, str_proc);

	while (YES)
	{
	}
}
