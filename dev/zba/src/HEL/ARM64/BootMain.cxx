/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <FirmwareKit/EFI.hxx>
#include <BootKit/Thread.hxx>
#include <BootKit/BootKit.hxx>

EXTERN EfiBootServices* BS;

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 Main(EfiHandlePtr	  ImageHandle,
							  EfiSystemTable* SystemTable)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"NEWSOLDR, (C) ZKA TECHNOLOGIES, ALL RIGHTS RESERVED.\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"NEWSOLDR: BOOTING KERNEL...\r\n");

    CANT_REACH();
}
