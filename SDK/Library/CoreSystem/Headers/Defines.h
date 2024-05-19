/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#ifdef CS_MUST_PASS
#undef CS_MUST_PASS
#endif

#ifdef _DEBUG
#define CS_MUST_PASS(e)                                                                                             \
	{                                                                                                               \
		if (!e)                                                                                                     \
		{                                                                                                           \
			Alert("Sorry, an assertion failed.\nFile: %s\nLine: %i", __FILE__, __LINE__) RtAssertTriggerInterrupt() \
		}                                                                                                           \
	}
#else
#define CS_MUST_PASS(e) CS_UNREFERENCED_PARAMETER(e)
#endif

#ifdef __cplusplus

#define CS_EXTERN_C extern "C"

#else

#define CS_EXTERN_C extern

#endif

struct ApplicationInterface;
struct GUID;

CS_EXTERN_C void RtAssertTriggerInterrupt(void);

#define CS_STDCALL __attribute__((stdcall))
#define CS_CDECL   __attribute__((cdecl))
#define CS_MSCALL  __attribute__((ms_abi))

#define PACKED __attribute__((packed))

#define CS_PASCAL CS_STDCALL

#include <Headers/Hint.h>

typedef __UINT8_TYPE__	ByteType;
typedef __UINT16_TYPE__ WordType;
typedef __UINT32_TYPE__ DWordType;
typedef __UINT64_TYPE__ QWordType;
typedef __SIZE_TYPE__	SizeType;

typedef char			   CharacterTypeUTF8;
typedef CharacterTypeUTF8* PtrCharacterType;

typedef void* PtrVoidType;
typedef void  VoidType;

#ifdef __SINGLE_PRECISION__
typedef float FloatType;
typedef float PositionType;
#else
typedef double FloatType;
typedef double PositionType;
#endif

typedef __UINTPTR_TYPE__ UIntPtrType;
typedef __INTPTR_TYPE__	 IntPtrType;
typedef __UINT64_TYPE__	 UInt64Type;
typedef __INT64_TYPE__	 Int64Type;
typedef __UINT32_TYPE__	 UInt32Type;
typedef __INT32_TYPE__	 Int32Type;

typedef CharacterTypeUTF8 BooleanType;

#define Yes 1
#define No	0

#define CS_PTR *

#define CS_UNREFERENCED_PARAMETER(e) ((VoidType)(e))

#ifdef __x86_64__

#define CS_FAR	__far
#define CS_NEAR __near

#define _M_AMD64 2
#else

#define CS_FAR
#define CS_NEAR

#endif

#ifdef __aarch64__
#define _M_AARCH64 3
#endif

#ifdef __powerpc64__
#define _M_PPC64 4
#endif

#ifdef __64x0__
#define _M_64000 5
#endif

#ifdef __riscv__
#define _M_RISCV 6
#endif

#define CS_STATIC static
#define CS_INLINE inline
#define CS_CONST  const

#ifdef __cplusplus
#define CS_CONSTEXPR constexpr
#else
#define CS_CONSTEXPR
#endif // __cplusplus

enum RtProcessCall
{
	kCallAllocPtr = 1,
	kCallFreePtr,
	kCallSizePtr,
	kCallCheckPtr,
	kCallAllocStack,
	/// @brief Open a specific handle
	/// (can be used as sel to call methods related to it.)
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
	kCallRandomNumberGenerator,
	kCallGetArgsCount,
	kCallGetArgsPtr,
	/// @brief Number of process calls.
	kCallsCount,
};

/**
 * @brief GUID type, something you can also find in CFKit.
 * @author Amlal El Mahrouss
 */
typedef struct GUID
{
	DWordType Data1;
	WordType  Data2;
	WordType  Data3;
	ByteType  Data4[8];
} GUIDType, *PtrGUIDType;

/// \brief Application Interface.
/// \author Amlal El Mahrouss
typedef struct ApplicationInterface
{
	VoidType (*Release)(struct ApplicationInterface* Self, DWordType ExitCode);
	IntPtrType (*Invoke)(struct ApplicationInterface* Self, DWordType Sel, ...);
	VoidType (*Query)(struct ApplicationInterface* Self, PtrVoidType* Dst, SizeType SzDst, struct GUID* GuidOf);
} ApplicationInterface, *ApplicationInterfaceRef;

#ifdef __cplusplus

#define CS_COPY_DELETE(KLASS)                \
	KLASS& operator=(const KLASS&) = delete; \
	KLASS(const KLASS&)			   = delete;

#define CS_COPY_DEFAULT(KLASS)                \
	KLASS& operator=(const KLASS&) = default; \
	KLASS(const KLASS&)			   = default;

#define CS_MOVE_DELETE(KLASS)           \
	KLASS& operator=(KLASS&&) = delete; \
	KLASS(KLASS&&)			  = delete;

#define CS_MOVE_DEFAULT(KLASS)           \
	KLASS& operator=(KLASS&&) = default; \
	KLASS(KLASS&&)			  = default;

#define app_cast reinterpret_cast<ApplicationInterfaceRef>

template <SizeType N>
using StrType = CharacterTypeUTF8[N];

#else

#define app_cast (ApplicationInterfaceRef)

#endif // ifdef C++

/// @brief Get app singleton.
/// @param
/// @return
CS_EXTERN_C ApplicationInterfaceRef RtGetAppPointer(VoidType);

/// @brief Get argument count
/// @param
/// @return
CS_EXTERN_C SizeType RtGetAppArgumentsCount(VoidType);

/// @brief Get argument pointer.
/// @param
/// @return
CS_EXTERN_C CharacterTypeUTF8** RtGetAppArgumentsPtr(VoidType);

CS_EXTERN_C ApplicationInterfaceRef kSharedApplication;

typedef CharacterTypeUTF8 StrType255[255];

#define True  1
#define False 0
#define Bool  BooleanType

#define NullPtr ((PtrVoidType)0)

#ifndef kInvalidRef
#define kInvalidRef 0
#endif

#include <Headers/Alert.h>
