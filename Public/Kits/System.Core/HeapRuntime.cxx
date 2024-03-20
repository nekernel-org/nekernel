/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Heap.hxx>

/// @brief Allocate from the user's heap.
/// @param refObj Process object.
/// @param sz size of object.
/// @param flags flags.
/// @return
CA_EXTERN_C PVOID HcAllocateProcessHeap(ObjectPtr refObj, QWORD sz,
                                        DWORD flags) {
  CA_MUST_PASS(sz);
  CA_MUST_PASS(flags);

  return (PVOID)refObj->Invoke(refObj, kProcessCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param refObj Process object.
/// @param ptr the pointer to free.
CA_EXTERN_C VOID HcFreeProcessHeap(ObjectPtr refObj, PVOID ptr) {
  CA_MUST_PASS(ptr);
  CA_UNREFERENCED_PARAMETER(refObj->Invoke(refObj, kProcessCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param refObj Process object.
/// @param ptr the pointer to find.
/// @return the size.
CA_EXTERN_C QWORD HcProcessHeapSize(ObjectPtr refObj, PVOID ptr) {
  CA_MUST_PASS(ptr);
  return refObj->Invoke(refObj, kProcessCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param refObj Process object.
/// @param ptr the pointer to check.
/// @return if it exists
CA_EXTERN_C BOOL HcProcessHeapExists(ObjectPtr refObj, PVOID ptr) {
  CA_MUST_PASS(ptr);
  return refObj->Invoke(refObj, kProcessCallCheckPtr, ptr);
}
