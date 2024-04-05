/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Main Application object, retrieved from the RtGetApp symbol.
ApplicationRef kSharedApplication = NullPtr;

/// @brief Gets the app arguments count.
/// @param void no arguments.
/// @return
CA_EXTERN_C SizeType RtGetAppArgumentsCount(VoidType) {
  return kSharedApplication->Invoke(kSharedApplication, kCallGetArgsCount);
}

/// @brief Gets the app arguments pointer.
/// @param void no arguments.
/// @return
CA_EXTERN_C CharacterTypeUTF8** RtGetAppArgumentsPtr(VoidType) {
  return (CharacterTypeUTF8**)kSharedApplication->Invoke(kSharedApplication,
                                                         kCallGetArgsPtr);
}
