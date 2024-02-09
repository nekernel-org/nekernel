/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <EFIKit/Api.hxx>
#include <EFIKit/EFI.hxx>

extern "C" Int32 EfiMain(EfiHandlePtr handle, EfiSystemTable* SystemTable) {
  return kEfiOk;
}
