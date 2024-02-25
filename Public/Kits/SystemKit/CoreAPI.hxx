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

#ifdef CA_MUST_PASS
#undef CA_MUST_PASS
#endif

// unused by user side, it's a kernel thing.
#define CA_MUST_PASS(e) ((void)e)

#define CA_EXTERN_C extern "C"

#define CA_STDCALL __attribute__((stdcall))
#define CA_CDECL __attribute__((cdecl))
#define CA_MSCALL __attribute__((ms_abi))

typedef __UINT8_TYPE__ BYTE;
typedef __UINT16_TYPE__ WORD;
typedef __UINT32_TYPE__ DWORD;
typedef __UINT64_TYPE__ QWORD;
