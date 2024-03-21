/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#ifndef __cplusplus
#error This API is meant to be used with C++.
#endif

#ifdef CA_MUST_PASS
#undef CA_MUST_PASS
#endif

#include <ObjectKit/ObjectKit.hxx>

#ifdef _DEBUG
#define CA_MUST_PASS(e) { if (!e) { __assert_chk_fail() } }
#else
#define CA_MUST_PASS(e) CA_UNREFERENCED_PARAMETER(e)
#endif

#ifdef __cplusplus

#define CA_EXTERN_C extern "C"

#else

#define CA_EXTERN_C extern

#endif

CA_EXTERN_C void __assert_chk_fail(void);

#define CA_STDCALL __attribute__((stdcall))
#define CA_CDECL __attribute__((cdecl))
#define CA_MSCALL __attribute__((ms_abi))

#define CA_PASCAL CA_STDCALL

typedef __UINT8_TYPE__ BYTE;
typedef __UINT16_TYPE__ WORD;
typedef __UINT32_TYPE__ DWORD;
typedef __UINT64_TYPE__ QWORD;

typedef char CHAR;
typedef CHAR* PCHAR;

typedef void* PVOID;
typedef void VOID;

typedef __UINTPTR_TYPE__ UINT_PTR;
typedef __INTPTR_TYPE__ INT_PTR;
typedef __UINT64_TYPE__ UINT64;
typedef __INT64_TYPE__ INT64;
typedef __UINT32_TYPE__ UINT32;
typedef __INT32_TYPE__ INT32;

typedef __WCHAR_TYPE__ WCHAR;
typedef WCHAR* PWCHAR;

typedef CHAR BOOL;

#define TRUE 1
#define FALSE 0

#define CA_PTR *

#define CA_FAR __far
#define CA_NEAR __near

#define CA_UNREFERENCED_PARAMETER(e) ((VOID)e)

#ifdef __x86_64__
#   define _M_AMD64 2
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

CA_INLINE ObjectPtr kInstanceObject;

enum HcProcessCall {
    kProcessCallAllocPtr = 1,
    kProcessCallFreePtr,
    kProcessCallSizePtr,
    kProcessCallCheckPtr,
    kProcessCallAllocStack,
    kProcessCallOpenHandle,
    kProcessCallCloseHandle,
    kProcessCallsCount = 7,
};

#include <System.Core/Hint.hxx>

class SystemException {
 public:
  explicit SystemException() = default;
  virtual ~SystemException() = default;

 public:
  HCORE_COPY_DEFAULT(SystemException);

 public:
  virtual const char *Name() = 0;
  virtual const char *Reason() = 0;

};

/// @brief Object exception
/// Throws when the object isn't found.
class ObjectNotFoundException : public SystemException {
 public:
  explicit ObjectNotFoundException() = default;
  virtual ~ObjectNotFoundException() = default;

 public:
  HCORE_COPY_DEFAULT(ObjectNotFoundException);

 public:
  const char *Name() override { return "ObjectNotFoundException"; } 
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{
      "System.Core: ObjectNotFoundException: Catastrophic failure!"};
};

/// @brief pointer exception
/// Throws when the object isn't found.
class PointerException : public SystemException {
 public:
  explicit PointerException() = default;
  virtual ~PointerException() = default;

 public:
  HCORE_COPY_DEFAULT(PointerException);

 public:
  const char *Name() override { return "PointerException"; } 
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{
      "System.Core: PointerException: Catastrophic failure!"};
};

/// @brief pointer exception
/// Throws when the object isn't found.
class NullPointerException : public SystemException {
 public:
  explicit NullPointerException() = default;
  virtual ~NullPointerException() = default;

 public:
  HCORE_COPY_DEFAULT(NullPointerException);

 public:
  const char *Name() override { return "NullPointerException"; } 
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{
      "System.Core: NullPointerException: Catastrophic failure!"};
};
