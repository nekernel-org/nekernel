/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#pragma once

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif // ifdef __cplusplus

typedef struct HcObject {
  void(*Release)(void);
  void(*Invoke)(void);
  void(*QueryInterface)(void);
} *HcObjectPtr;

EXTERN_C HcObjectPtr HcGetProcessHeap(void);
EXTERN_C void* HcAllocateProcessHeap(HcObjectPtr refObj, long long int sz, int flags);
EXTERN_C void HcFreeProcessHeap(HcObjectPtr refObj, void* ptr);
EXTERN_C long long int HcProcessHeapSize(HcObjectPtr refObj, void* ptr);
EXTERN_C long long int HcProcessHeapExists(HcObjectPtr refObj, void* ptr);