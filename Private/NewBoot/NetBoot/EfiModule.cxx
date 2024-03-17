/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FirmwareKit/EFI.hxx>

EXTERN_C Int32 EfiMain(EfiHandlePtr handle, EfiSystemTable* SystemTable) 
{
  InitEFI(ST);

  /// - Find a network drive called ".\\HCoreWorkgroup"
  /// - Download our image
  /// - Boot from it.

  return kEfiOk;
}
