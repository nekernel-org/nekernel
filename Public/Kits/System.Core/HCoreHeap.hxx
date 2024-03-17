/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#pragma once

#include <System.Core/HCoreBase.hxx>

CA_EXTERN_C ObjectPtr HcGetProcessObject(void);
CA_EXTERN_C PVOID HcAllocateProcessHeap(ObjectPtr refObj, QWORD sz, DWORD flags);
CA_EXTERN_C VOID HcFreeProcessHeap(ObjectPtr refObj, PVOID ptr);
CA_EXTERN_C QWORD HcProcessHeapSize(ObjectPtr refObj, PVOID ptr);
CA_EXTERN_C QWORD HcProcessHeapExists(ObjectPtr refObj, PVOID ptr);
