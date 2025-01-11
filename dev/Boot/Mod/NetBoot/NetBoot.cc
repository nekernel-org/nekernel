/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024, t& Corporation, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#include <Mod/NetBoot/NetBoot.h>
#include <BootKit/BootKit.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
#ifdef __ZKA_AMD64__
	return kEfiFail;
#else
	return kEfiOk;
#endif
}
