/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Application entrypoint.
/// @param void
/// @return void
CS_EXTERN_C VoidType AppMain(VoidType);

/// @brief Process entrypoint.
/// @param void
/// @return void
CS_EXTERN_C VoidType __ImageStart(VoidType)
{
	kSharedApplication = RtGetAppPointer();
	CS_MUST_PASS(kSharedApplication);

	AppMain();
}
