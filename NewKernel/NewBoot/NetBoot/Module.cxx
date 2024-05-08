/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright SoftwareLabs, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>

EXTERN_C Int32 EfiMain(Void)
{
	/// - Find a network drive called "/OnlineBoot"
	/// - Download our image
	/// - Boot from it.

	return kEfiOk;
}
