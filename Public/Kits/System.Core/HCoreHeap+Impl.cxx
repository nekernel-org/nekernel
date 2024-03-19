/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/HCoreHeap.hxx>

/// @brief Allocate from the user's heap.
/// @param refObj 
/// @param sz 
/// @param flags 
/// @return 
CA_EXTERN_C PVOID HcAllocateProcessHeap(ObjectPtr refObj, QWORD sz, DWORD flags)
{
    return (PVOID)refObj->Invoke(refObj, kProcessCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param refObj 
/// @param ptr 
/// @return 
CA_EXTERN_C VOID HcFreeProcessHeap(ObjectPtr refObj, PVOID ptr)
{
    CA_UNREFERENCED_PARAMETER(refObj->Invoke(refObj, kProcessCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param refObj 
/// @param ptr 
/// @return 
CA_EXTERN_C QWORD HcProcessHeapSize(ObjectPtr refObj, PVOID ptr)
{
    return refObj->Invoke(refObj, kProcessCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param refObj Process object.
/// @param ptr 
/// @return 
CA_EXTERN_C QWORD HcProcessHeapExists(ObjectPtr refObj, PVOID ptr)
{
    return refObj->Invoke(refObj, kProcessCallCheckPtr, ptr);
}

// EOF.
