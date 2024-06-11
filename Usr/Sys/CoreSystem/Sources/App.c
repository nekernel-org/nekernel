/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Main Application object, retrieved from the RtGetAppPointer symbol.
ApplicationInterfaceRef kSharedApplication = NullPtr;

/// @brief Gets the app arguments count.
/// @param void no arguments.
/// @return The number of arguments given to the application.
CS_EXTERN_C SizeType RtGetAppArgumentsCount(VoidType)
{
	CS_MUST_PASS(kSharedApplication);

	return kSharedApplication->Invoke(kSharedApplication, kCallGetArgsCount);
}

/// @brief Gets the app arguments pointer.
/// @param void no arguments.
/// @return
CS_EXTERN_C CharacterTypeUTF8** RtGetAppArgumentsPtr(VoidType)
{
	CS_MUST_PASS(kSharedApplication);

	return (CharacterTypeUTF8**)kSharedApplication->Invoke(kSharedApplication,
														   kCallGetArgsPtr);
}
