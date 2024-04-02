/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Heap.h>

/// @brief Allocate from the user's heap.
/// @param sz size of object.
/// @param flags flags.
/// @return
CA_EXTERN_C PtrVoidType RtTlsAllocate(QWordType sz,
                                            DWordType flags) {
  CA_MUST_PASS(sz);
  CA_MUST_PASS(flags);

  return (PtrVoidType)kSharedApplication->Invoke(kSharedApplication, kCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param ptr the pointer to free.
CA_EXTERN_C VoidType RtTlsFree(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  CA_UNREFERENCED_PARAMETER(kSharedApplication->Invoke(kSharedApplication, kCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param ptr the pointer to find.
/// @return the size.
CA_EXTERN_C QWordType RtTlsGetSize(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return kSharedApplication->Invoke(kSharedApplication, kCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param ptr the pointer to check.
/// @return if it exists
CA_EXTERN_C BooleanType RtTlsPtrExists(PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return kSharedApplication->Invoke(kSharedApplication, kCallCheckPtr, ptr);
}
