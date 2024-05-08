/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Main Application object, retrieved from the RtGetAppPointer symbol.
ApplicationInterfaceRef kSharedApplication = NullPtr;

/// @brief Gets the app arguments count.
/// @param void no arguments.
/// @return The number of arguments given to the application.
CA_EXTERN_C SizeType RtGetAppArgumentsCount(VoidType)
{
	CA_MUST_PASS(kSharedApplication);

	return kSharedApplication->Invoke(kSharedApplication, kCallGetArgsCount);
}

/// @brief Gets the app arguments pointer.
/// @param void no arguments.
/// @return
CA_EXTERN_C CharacterTypeUTF8** RtGetAppArgumentsPtr(VoidType)
{
	CA_MUST_PASS(kSharedApplication);

	return (CharacterTypeUTF8**)kSharedApplication->Invoke(kSharedApplication,
														   kCallGetArgsPtr);
}
