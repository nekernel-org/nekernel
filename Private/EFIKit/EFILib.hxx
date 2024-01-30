/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __EFI_LIB__
#define __EFI_LIB__

#include <EFIKit/EFI.hxx>

#include "NewKit/Defines.hpp"

inline EfiSystemTable* ST = nullptr;
inline EfiBootServices* BS = nullptr;

namespace EFI {
/**
@brief Stop Execution of program.
@param SystemTable EFI System Table.
*/
inline Void Stop(EfiSystemTable* SystemTable) noexcept {
  while (true) {
    rt_cli();
    rt_halt();
  }
}

/**
@brief Exit EFI API to let the OS load correctly.
Bascially frees everything we have in the EFI side.
*/
inline void ExitBootServices(EfiSystemTable* SystemTable, UInt64 MapKey,
                             EfiHandlePtr ImageHandle) noexcept {
  SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
}
}  // namespace EFI

inline void KeInitEFI(EfiSystemTable* SystemTable) noexcept {
  ST = SystemTable;
  BS = ST->BootServices;
}

inline void KeRuntimeStop(const EfiCharType* File,
                          const EfiCharType* Reason) noexcept {
  ST->ConOut->OutputString(ST->ConOut, L"*** STOP ***\r\n");

  ST->ConOut->OutputString(ST->ConOut, L"*** File:");
  ST->ConOut->OutputString(ST->ConOut, File);
  ST->ConOut->OutputString(ST->ConOut, L", Reason:");
  ST->ConOut->OutputString(ST->ConOut, Reason);
  ST->ConOut->OutputString(ST->ConOut, L" ***\r\n");

  EFI::Stop(ST);
}

#ifdef __BOOTLOADER__
#include <BootKit/Platform.hxx>
#endif  // IF TARGET=BOOTLOADER

#endif /* ifndef __EFI_LIB__ */
