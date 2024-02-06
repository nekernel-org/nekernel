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
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>

inline EfiSystemTable *ST = nullptr;
inline EfiBootServices *BS = nullptr;

namespace EFI {
/**
@brief Stop Execution of program.
*/
Void Stop() noexcept;

/**
@brief Exit EFI API to let the OS load correctly.
Bascially frees everything we have in the EFI side.
*/
inline void ExitBootServices(UInt64 MapKey, EfiHandlePtr ImageHandle) noexcept {
  if (!ST) return;

  ST->ConOut->OutputString(ST->ConOut, L"HCoreLdr: Exit BootServices...\r\n");
  ST->BootServices->ExitBootServices(ImageHandle, MapKey);
}

enum {
  kPartEPM,
  kPartGPT,
  kPartMBR,
  kPartCnt,
};

inline UInt32 Platform() noexcept { return kPEMachineAMD64; }

/***
 * @brief Raise Hard kernel error.
 */
inline void RaiseHardError(const EfiCharType *ErrorCode,
                           const EfiCharType *Reason) noexcept {
  ST->ConOut->OutputString(ST->ConOut, L"*** STOP ***\r\n");

  ST->ConOut->OutputString(ST->ConOut, L"*** Error: ");
  ST->ConOut->OutputString(ST->ConOut, ErrorCode);

#ifdef __DEBUG__
  ST->ConOut->OutputString(ST->ConOut, L", Reason: ");
  ST->ConOut->OutputString(ST->ConOut, Reason);
#endif  // ifdef __DEBUG__

  ST->ConOut->OutputString(ST->ConOut, L" ***\r\n");

  EFI::Stop();
}
}  // namespace EFI

inline void InitEFI(EfiSystemTable *SystemTable) noexcept {
  if (!SystemTable) return;

  ST = SystemTable;
  BS = ST->BootServices;

  ST->ConOut->ClearScreen(SystemTable->ConOut);
  ST->ConOut->SetAttribute(SystemTable->ConOut, kEFIYellow);

  ST->BootServices->SetWatchdogTimer(0, 0, 0, nullptr);
}

#ifdef __BOOTLOADER__

#include <BootKit/Platform.hxx>

#endif  // ifdef __BOOTLOADER__

#endif /* ifndef __EFI_API__ */
