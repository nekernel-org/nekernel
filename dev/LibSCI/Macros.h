/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

File: Macros.h
Purpose: LibSCI Macros header.

------------------------------------------- */

#pragma once

/***********************************************************************************/
/// @file LibSCI/Macros.h
/// @brief Macros and core types.
/***********************************************************************************/

#include <CompilerHint.h>

#define ATTRIBUTE(X) __attribute__((X))

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

#define DEPRECATED ATTRIBUTE(deprecated)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define FILE_MAX_LEN 256

typedef bool Bool;
typedef void Void;

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

typedef VoidPtr SCIObject;

typedef SCIObject IOObject;
typedef IOObject  FSObject;
typedef SCIObject DLLObject;
typedef SCIObject ThreadObject;
typedef SCIObject SocketObject;

#ifdef __cplusplus
typedef decltype(nullptr) nullPtr;
typedef decltype(nullptr) NullPtr;
#endif

EXTERN_C void _rtl_assert(Bool expr, const Char* origin);

#define MUST_PASS(X) _rtl_assert(X, __FILE__)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)              \
	(((sizeof(a) / sizeof(*(a))) / \
	  (static_cast<Kernel::Size>(!(sizeof(a) % sizeof(*(a)))))))
#endif