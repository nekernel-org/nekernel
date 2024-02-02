/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __EFI_API__
#define __EFI_API__

#include <EFIKit/EFI.hxx>

inline EfiSystemTable* ST = nullptr;
inline EfiBootServices* BS = nullptr;

namespace EFI {
/**
@brief Stop Execution of program.
*/
inline Void Stop() noexcept {
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
  if (!SystemTable) return;

  SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
}
}  // namespace EFI

inline void KeInitEFI(EfiSystemTable* SystemTable) noexcept {
  if (!SystemTable) return;

  ST = SystemTable;
  BS = ST->BootServices;

  ST->ConOut->ClearScreen(SystemTable->ConOut);
  ST->ConOut->SetAttribute(SystemTable->ConOut, kEFIYellow);
}

inline void KeRuntimeStop(const EfiCharType* ErrorCode,
                          const EfiCharType* Reason) noexcept {
  ST->ConOut->OutputString(ST->ConOut, L"*** STOP ***\r\n");

  ST->ConOut->OutputString(ST->ConOut, L"*** ErrorCode: ");
  ST->ConOut->OutputString(ST->ConOut, ErrorCode);
  ST->ConOut->OutputString(ST->ConOut, L", Reason: ");
  ST->ConOut->OutputString(ST->ConOut, Reason);
  ST->ConOut->OutputString(ST->ConOut, L" ***\r\n");

  EFI::Stop();
}

enum {
  kPartEPM,
  kPartGPT,
  kPartMBR,
  kPartCnt,
};

#ifdef __BOOTLOADER__
#include <BootKit/Platform.hxx>
#endif  // IF TARGET=BOOTLOADER

#endif /* ifndef __EFI_API__ */
