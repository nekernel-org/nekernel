/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Headers/Heap.h>

/// @brief Allocate from the user's heap.
/// @param sz size of object.
/// @param flags flags.
/// @return
CA_EXTERN_C PtrVoidType RtAllocateProcessPtr(QWordType sz,
                                            DWordType flags) {
  CA_MUST_PASS(sz);
  CA_MUST_PASS(flags);

  return (PtrVoidType)kApplicationObject->Invoke(kApplicationObject, kCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param ptr the pointer to free.
CA_EXTERN_C VoidType RtFreeProcessPtr(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  CA_UNREFERENCED_PARAMETER(kApplicationObject->Invoke(kApplicationObject, kCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param ptr the pointer to find.
/// @return the size.
CA_EXTERN_C QWordType RtProcessPtrSize(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return kApplicationObject->Invoke(kApplicationObject, kCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param ptr the pointer to check.
/// @return if it exists
CA_EXTERN_C BooleanType RtProcessPtrExists(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return kApplicationObject->Invoke(kApplicationObject, kCallCheckPtr, ptr);
}
