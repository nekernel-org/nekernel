/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <FirmwareKit/EFI.hxx>

EXTERN EfiBootServices* BS;

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 Main(EfiHandlePtr	  ImageHandle,
							  EfiSystemTable* SystemTable)
{

    return 0;
}
