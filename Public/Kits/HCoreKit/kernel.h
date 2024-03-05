/** 
*   The HCore Kit
*   Copyright Mahrouss Logic
*   File: HKernel.h
*   Purpose: Base HCore header
*/

#pragma once

/* process id */
typedef long rt_kernel_port;

/* @brief scheduling team */
typedef long rt_kernel_team;

/* virtual memory key */
typedef long long int rt_virt_mem_t;

/// override previous vm size if any.

#ifdef kVirtualMemorySize
// do not edit this! if you want to avoid your program crashing.
#undef kVirtualMemorySize
#endif /* ifdef kVirtualMemorySize */

/// 4 megs of additional memory.
#define kVirtualMemorySize 4096U
