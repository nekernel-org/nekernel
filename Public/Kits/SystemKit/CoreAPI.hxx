/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#ifndef __cplusplus
#error This API is meant to be used with C++
#endif

#ifdef MUST_PASS
#undef MUST_PASS
#endif

// unused by user side, it's a kernel thing.
#define MUST_PASS(e) ((void)e)

#define EXTERN_C extern "C"

#define STDCALL __attribute__((stdcall))
#define CDECL __attribute__((cdecl))
#define MSCALL __attribute__((ms_abi))

typedef __UINT16_TYPE__ WORD;
typedef __UINT32_TYPE__ DWORD;
typedef __UINT64_TYPE__ QWORD;
