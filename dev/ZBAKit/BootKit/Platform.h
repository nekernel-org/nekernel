/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/**
	@file Platform.h
	@brief Platform specific code.
*/

#ifdef __x86_64__

#ifdef __cplusplus
#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif
#else
#ifndef EXTERN_C
#define EXTERN_C extern
#endif
#endif // __cplusplus

EXTERN_C void rt_halt();
EXTERN_C void rt_cli();
EXTERN_C void rt_sti();
EXTERN_C void rt_cld();
EXTERN_C void rt_std();

#endif /* ifdef __x86_64__ */
