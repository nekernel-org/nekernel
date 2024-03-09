/*
 *	========================================================
 *
 *	CxxKit
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FirmwareKit/EFI.hxx>

namespace cxxkit 
{
///! @brief C++ ABI unwinding
///! Fini array (r1)
///! Numbers of Fini (r2)
EXTERN_C void __unwind(void (**finis)(void), int cnt) 
{
  for (int i = 0; i < cnt; ++i) 
    (finis[i])();
}
}  // namespace cxxkit

EXTERN_C Int32 EfiMain(EfiHandlePtr handle, EfiSystemTable* SystemTable) 
{
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"HCoreLdr: C++ Runtime Add-in\r\n");
  
  return kEfiOk;
}