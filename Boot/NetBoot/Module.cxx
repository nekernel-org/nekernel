/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright SoftwareLabs, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <NetBoot/NetBoot.hxx>

EXTERN_C Int32 ModuleMain(Void)
{
	/// - Find a network drive called "/Remote/NewOSKrnl"
	/// - Download our image
	/// - Boot from it.

	return kEfiOk;
}
