/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright ZKA Technologies., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <NetBoot/NetBoot.hxx>

EXTERN EfiSystemTable*	ST;

EXTERN_C Int32 ModuleMain(Void)
{
	ST->ConOut->ClearScreen(ST->ConOut);

	/// @todo
	/// - Find a network drive called "\.\newoskrnl.dll" with fork 'For-NewOSLdr' in the network path. (NewFS formatted disk as well!)
	/// - Download our image
	/// - Boot from it.

	return kEfiOk;
}
