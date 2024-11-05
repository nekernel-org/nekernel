#pragma once

// ------------------------------------------------------------------------------------------ //
/// @brief Handle Type and Macros definitions.
// ------------------------------------------------------------------------------------------ //

#define ATTRIBUTE(X) __attribute__((X))
#define IMPORT_CXX	 extern "C++"
#define IMPORT_C	 extern "C"

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

typedef SCIObject DLLObject;
typedef SCIObject IOObject;
typedef SCIObject SCMObject;
typedef SCIObject ThreadObject;
typedef SCIObject SocketObject;
typedef SCIObject ShellObject;
typedef SCIObject UIObject;
