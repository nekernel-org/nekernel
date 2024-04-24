/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __EFI_API__
#define __EFI_API__

#ifdef __NEWBOOT__
#include <BootKit/Rsrc/NewBootFatal.rsrc>
#include <Builtins/Toolbox/Toolbox.hxx>
#endif // ifdef __NEWBOOT__

#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hxx>

inline EfiSystemTable *ST = nullptr;
inline EfiBootServices *BS = nullptr;

EXTERN_C void rt_cli();
EXTERN_C void rt_hlt();

namespace EFI {
/// @brief Halt and clear interrupts.
/// @return
inline Void Stop() noexcept {
  while (1) {
    rt_hlt();
    rt_cli();
  }
}

/**
@brief Exit EFI API to let the OS load correctly.
Bascially frees everything we have in the EFI side.
*/
inline void ExitBootServices(UInt64 MapKey, EfiHandlePtr ImageHandle) noexcept {
  if (!ST) return;

  while (ST->BootServices->ExitBootServices(ImageHandle, MapKey) != kEfiOk)
    ;
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
  ST->ConOut->OutputString(ST->ConOut, L"\r\n*** STOP ***\r\n");

  ST->ConOut->OutputString(ST->ConOut, L"*** Error: ");
  ST->ConOut->OutputString(ST->ConOut, ErrorCode);

#ifdef __DEBUG__
  ST->ConOut->OutputString(ST->ConOut, L", Reason: ");
  ST->ConOut->OutputString(ST->ConOut, Reason);
#endif  // ifdef __DEBUG__

  ST->ConOut->OutputString(ST->ConOut, L" ***\r\n");

#ifdef __NEWBOOT__
  ToolboxInitRsrc();

  ToolboxDrawRsrc(NewBootFatal, NEWBOOTFATAL_HEIGHT, NEWBOOTFATAL_WIDTH,
                  (kHandoverHeader->f_GOP.f_Width - NEWBOOTFATAL_WIDTH) / 2,
                  (kHandoverHeader->f_GOP.f_Height - NEWBOOTFATAL_HEIGHT) / 2);

  ToolboxClearRsrc();
#endif // ifdef __NEWBOOT__

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
  ST->ConOut->EnableCursor(ST->ConOut, false);
}

#ifdef __BOOTLOADER__

#include <BootKit/Platform.hxx>

#endif  // ifdef __BOOTLOADER__

#define kNewOSSubsystem 17

#endif /* ifndef __EFI_API__ */
