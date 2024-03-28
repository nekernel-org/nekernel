/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Headers/Defines.h>

/// @brief Gets the app arguments count.
/// @param
/// @return
CA_EXTERN_C SizeType RtGetAppArgumentsCount(VoidType) {
  return kApplicationObject->Invoke(kApplicationObject, kCallGetArgsCount);
}

/// @brief Gets the app arguments pointer.
/// @param
/// @return
CA_EXTERN_C CharacterTypeUTF8* RtGetAppArgumentsPtr(VoidType) {
  return (CharacterTypeUTF8*)kApplicationObject->Invoke(kApplicationObject,
                                                        kCallGetArgsPtr);
}