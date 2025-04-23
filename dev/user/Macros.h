/* -------------------------------------------

Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

File: Macros.h
Purpose: libsci Macros header.

------------------------------------------- */

#pragma once

/***********************************************************************************/
/// @file user/Macros.h
/// @brief Macros and Core types of the SCI (System Call Interface).
/***********************************************************************************/

#include <hint/CompilerHint.h>

#define ATTRIBUTE(X) __attribute__((X))

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

#define DEPRECATED ATTRIBUTE(deprecated)

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (1)

#define FILE_MAX_LEN (256)

#ifndef BOOL
#define BOOL bool
#endif

typedef bool Bool;
typedef bool Boolean;
typedef void Void;

#ifndef __cplusplus
#define true (1)
#define false (0)
#endif

#define YES true
#define NO	false

typedef __UINT64_TYPE__ UInt64;
typedef __UINT32_TYPE__ UInt32;
typedef __UINT16_TYPE__ UInt16;
typedef __UINT8_TYPE__	UInt8;

typedef __SIZE_TYPE__ SizeT;

typedef __INT64_TYPE__ SInt64;
typedef __INT32_TYPE__ SInt32;
typedef __INT16_TYPE__ SInt16;
typedef __INT8_TYPE__  SInt8;

typedef void*			 VoidPtr;
typedef __UINTPTR_TYPE__ UIntPtr;
typedef char			 Char;

#ifdef __cplusplus
typedef decltype(nullptr) nullPtr;
typedef nullPtr			  NullPtr;

#define SCI_COPY_DELETE(KLASS)               \
	KLASS& operator=(const KLASS&) = delete; \
	KLASS(const KLASS&)			   = delete;

#define SCI_COPY_DEFAULT(KLASS)               \
	KLASS& operator=(const KLASS&) = default; \
	KLASS(const KLASS&)			   = default;

#define SCI_MOVE_DELETE(KLASS)          \
	KLASS& operator=(KLASS&&) = delete; \
	KLASS(KLASS&&)			  = delete;

#define SCI_MOVE_DEFAULT(KLASS)          \
	KLASS& operator=(KLASS&&) = default; \
	KLASS(KLASS&&)			  = default;

#endif

IMPORT_C void _rtl_assert(Bool expr, const Char* origin);

#define MUST_PASS(X) _rtl_assert(X, __FILE__)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(X)              \
	(((sizeof(X) / sizeof(*(X))) / \
	  (static_cast<SizeT>(!(sizeof(X) % sizeof(*(X)))))))
#endif

#ifndef KIB
#define KIB(X) (UInt64)((X) / 1024)
#endif

#ifndef kib_cast
#define kib_cast(X) (UInt64)((X)*1024)
#endif

#ifndef MIB
#define MIB(X) (UInt64)((UInt64)KIB(X) / 1024)
#endif

#ifndef mib_cast
#define mib_cast(X) (UInt64)((UInt64)kib_cast(X) * 1024)
#endif

#ifndef GIB
#define GIB(X) (UInt64)((UInt64)MIB(X) / 1024)
#endif

#ifndef gib_cast
#define gib_cast(X) (UInt64)((UInt64)mib_cast(X) * 1024)
#endif

#ifndef TIB
#define TIB(X) (UInt64)((UInt64)GIB(X) / 1024)
#endif

#ifndef tib_cast
#define tib_cast(X) ((UInt64)gib_cast(X) * 1024)
#endif

#define SCI_UNUSED(X) ((void)X)
