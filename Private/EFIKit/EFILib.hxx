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

inline EfiSystemTable* ST = nullptr;
inline EfiBootServices* BS = nullptr;

namespace Detail {
/**
@brief Stop Execution of Bootloader.
@param SystemTable EFI System Table.
*/
Void Stop(EfiSystemTable* SystemTable) noexcept {
  while (true) {
    rt_cli();
    rt_halt();
  }
}
}  // namespace Detail

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

  Detail::Stop(ST);
}

#ifdef __BOOTLOADER__
#include <BootKit/Processor.hxx>
#endif  // IF TARGET=BOOTLOADER

#endif /* ifndef __EFI_LIB__ */
