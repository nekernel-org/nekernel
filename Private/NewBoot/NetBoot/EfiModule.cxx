/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FirmwareKit/EFI.hxx>

extern "C" Int32 EfiMain(EfiHandlePtr handle, EfiSystemTable* SystemTable) {
  return kEfiOk;
}
