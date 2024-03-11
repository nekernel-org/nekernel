/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#pragma once

#include <System.Core/HCoreBase.hxx>

CA_EXTERN_C HcObjectPtr HcGetProcessHeap(void);
CA_EXTERN_C void* HcAllocateProcessHeap(HcObjectPtr refObj, long long int sz, int flags);
CA_EXTERN_C void HcFreeProcessHeap(HcObjectPtr refObj, void* ptr);
CA_EXTERN_C long long int HcProcessHeapSize(HcObjectPtr refObj, void* ptr);
CA_EXTERN_C long long int HcProcessHeapExists(HcObjectPtr refObj, void* ptr);
