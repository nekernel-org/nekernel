/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FirmwareKit/EFI.hxx>
#include <BootKit/BootKit.hxx>

EXTERN_C Int32 EfiMain(EfiHandlePtr handle, EfiSystemTable* SystemTable) 
{
  InitEFI(ST);
  InitGOP();

  /// - Find a network drive called "/OnlineInstall"
  /// - Download our image
  /// - Boot from it.

  BTextWriter writer;
  writer.Write(L"NetBoot.exe: Updating from OTP...\r\n");

  return kEfiOk;
}
