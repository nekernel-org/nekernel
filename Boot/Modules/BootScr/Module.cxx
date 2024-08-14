/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright ZKA Technologies., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>

EXTERN EfiSystemTable*	ST;

EXTERN_C Int32 ModuleMain(Void)
{
	ST->ConOut->ClearScreen(ST->ConOut);

	return kEfiOk;
}
