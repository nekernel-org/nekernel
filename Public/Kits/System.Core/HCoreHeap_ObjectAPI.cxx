/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/HCoreHeap.hxx>

CA_EXTERN_C PVOID HcAllocateProcessHeap(ObjectPtr refObj, QWORD sz, DWORD flags)
{
    return (PVOID)refObj->Invoke(refObj, kProcessHeapCallAlloc, sz, flags);
}

CA_EXTERN_C VOID HcFreeProcessHeap(ObjectPtr refObj, PVOID ptr)
{
    CA_UNREFERENCED_PARAMETER(refObj->Invoke(refObj, kProcessHeapCallFree, ptr));
}

CA_EXTERN_C QWORD HcProcessHeapSize(ObjectPtr refObj, PVOID ptr)
{
    return refObj->Invoke(refObj, kProcessHeapCallSize, ptr);
}

CA_EXTERN_C QWORD HcProcessHeapExists(ObjectPtr refObj, PVOID ptr)
{
    return refObj->Invoke(refObj, kProcessHeapCallCheck, ptr);
}

// eof.
