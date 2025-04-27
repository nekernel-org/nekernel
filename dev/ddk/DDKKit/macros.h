/* -------------------------------------------

  Copyright 2025 Amlal El Mahrouss.

  FILE: ddk.h
  PURPOSE: DDK Driver model base header.

------------------------------------------- */

#pragma once

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
#define BOOL bool
#define YES true
#define NO false
#define DDK_EXTERN extern "C" __declspec(dllexport)
#define nil nullptr
#undef NULL
#define NULL 0
#define DDK_FINAL final
#else
#define BOOL char
#define YES 1
#define NO 0
#define DDK_EXTERN extern __declspec(dllexport)
#define nil ((void*) 0)
#undef NULL
#define NULL ((void*) 0)
#define DDK_FINAL
#endif  // defined(__cplusplus)

#ifndef __DDK__
#undef DDK_EXTERN
#if defined(__cplusplus)
#define DDK_EXTERN extern "C" __declspec(dllimport)
#else
#define DDK_EXTERN __declspec(dllimport)
#endif
#endif

#define ATTRIBUTE(X) __attribute__((X))

#ifndef __NEOSKRNL__
#error !!! Do not include header in EL0/Ring 3 mode !!!
#endif  // __MINOSKRNL__