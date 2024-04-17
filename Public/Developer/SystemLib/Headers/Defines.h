/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#ifdef CA_MUST_PASS
#undef CA_MUST_PASS
#endif

#ifdef _DEBUG
#define CA_MUST_PASS(e) { if (!e) { DlgMsgBox("Sorry, an assertion failed.\nFile: %s\nLine: %i", __FILE__, __LINE__) RtAssertTriggerInterrupt() } }
#else
#define CA_MUST_PASS(e) CA_UNREFERENCED_PARAMETER(e)
#endif

#ifdef __cplusplus

#define CA_EXTERN_C extern "C"

#else

#define CA_EXTERN_C extern

#endif

struct Application;
struct GUID;

CA_EXTERN_C void RtAssertTriggerInterrupt(void);

#define CA_STDCALL __attribute__((stdcall))
#define CA_CDECL __attribute__((cdecl))
#define CA_MSCALL __attribute__((ms_abi))

#define PACKED __attribute__((packed))

#define CA_PASCAL CA_STDCALL

typedef __UINT8_TYPE__ ByteType;
typedef __UINT16_TYPE__ WordType;
typedef __UINT32_TYPE__ DWordType;
typedef __UINT64_TYPE__ QWordType;
typedef __SIZE_TYPE__ SizeType;

typedef char CharacterTypeUTF8;
typedef CharacterTypeUTF8* PtrCharacterType;

typedef void* PtrVoidType;
typedef void VoidType;

#ifdef __SINGLE_PRECISION__
typedef float PositionType;
#else
typedef double PositionType;
#endif

typedef __UINTPTR_TYPE__ UIntPtrType;
typedef __INTPTR_TYPE__ IntPtrType;
typedef __UINT64_TYPE__ UInt64Type;
typedef __INT64_TYPE__ Int64Type;
typedef __UINT32_TYPE__ UInt32Type;
typedef __INT32_TYPE__ Int32Type;

typedef CharacterTypeUTF8 BooleanType;

#define Yes 1
#define No 0

#define CA_PTR *

#define CA_UNREFERENCED_PARAMETER(e) ((VoidType)(e))

#ifdef __x86_64__

#   define CA_FAR __far
#   define CA_NEAR __near

#   define _M_AMD64 2
#else

#   define CA_FAR
#   define CA_NEAR

#endif 

#ifdef __aarch64__
#   define _M_AARCH64 3
#endif 

#ifdef __powerpc64__
#   define _M_PPC64 4 
#endif 

#ifdef __64x0__
#   define _M_64000 5 
#endif 

#ifdef __riscv__
#   define _M_RISCV 6
#endif 

#define CA_STATIC static
#define CA_INLINE inline
#define CA_CONST const

#ifdef __cplusplus
#define CA_CONSTEXPR constexpr
#else
#define CA_CONSTEXPR
#endif // __cplusplus

enum RtProcessCall {
    kCallAllocPtr = 1,
    kCallFreePtr,
    kCallSizePtr,
    kCallCheckPtr,
    kCallAllocStack,
    /// @brief Open a specific handle (can be used as sel to call methods related to it.)
    kCallOpenFile,
    kCallCloseFile,
    kCallOpenDir,
    kCallCloseDir,
    kCallOpenDevice,
    kCallCloseDevice,
    kCallCreateWindow,
    kCallCloseWindow,
    kCallCreateMenu,
    kCallCloseMenu,
    kCallGetArgsCount,
    kCallGetArgsPtr,
    /// @brief Number of process calls.
    kCallsCount,
};

#include <Headers/Hint.h>
#include <Headers/Dialog.h>

/**
 * @brief GUID type, something you can also find in CFKit.
 * @author Amlal El Mahrouss
 */
typedef struct GUID {
  DWordType Data1;
  WordType Data2;
  WordType Data3;
  ByteType Data4[8];
} GUIDType, *PtrGUIDType;

/// \brief Application Interface.
/// \author Amlal El Mahrouss
typedef struct Application {
  VoidType(*Release)(struct Application* Self, DWordType ExitCode);
  IntPtrType(*Invoke)(struct Application* Self, DWordType Sel, ...);
  VoidType(*Query)(struct Application* Self, PtrVoidType* Dst, SizeType SzDst, struct GUID* GuidOf);
} Application, *ApplicationRef;

#ifdef __cplusplus

#define CA_COPY_DELETE(KLASS)                                                                                         \
    KLASS &operator=(const KLASS &) = delete;                                                                          \
    KLASS(const KLASS &) = delete;


#define CA_COPY_DEFAULT(KLASS)                                                                                        \
    KLASS &operator=(const KLASS &) = default;                                                                         \
    KLASS(const KLASS &) = default;


#define CA_MOVE_DELETE(KLASS)                                                                                         \
    KLASS &operator=(KLASS &&) = delete;                                                                               \
    KLASS(KLASS &&) = delete;


#define CA_MOVE_DEFAULT(KLASS)                                                                                        \
    KLASS &operator=(KLASS &&) = default;                                                                              \
    KLASS(KLASS &&) = default;


#define app_cast reinterpret_cast<ApplicationRef>

template <SizeType N>
using StrType = CharacterTypeUTF8[N];

#else

#define app_cast (ApplicationRef)

#endif // ifdef C++

/// @brief Get app singleton.
/// @param  
/// @return 
CA_EXTERN_C ApplicationRef     RtGetAppPointer(VoidType);

/// @brief Get argument count
/// @param  
/// @return 
CA_EXTERN_C SizeType           RtGetAppArgumentsCount(VoidType);

/// @brief Get argument pointer.
/// @param  
/// @return 
CA_EXTERN_C CharacterTypeUTF8** RtGetAppArgumentsPtr(VoidType);

CA_EXTERN_C ApplicationRef kSharedApplication;

typedef CharacterTypeUTF8 StrType255[255];

#define True  1
#define False 0
#define Bool BooleanType

#define NullPtr ((PtrVoidType)0)

#ifndef kInvalidRef
#define kInvalidRef 0
#endif
