/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kRTLMag0 'h'
#define kRTLMag1 'C'
#define kRTLMag2 'o'

template <typename T>
T* hcore_tls_new_ptr(void);

template <typename T>
bool hcore_tls_delete_ptr(T* ptr);

template <typename T, typename... Args>
T* hcore_tls_new_class(Args&&... args);

typedef char rt_cookie_type[3];

/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, Virtual Address 0x10000 (64x0, 32x0, ARM64)
struct ThreadInformationBlock final
{
    hCore::Char Name[255]; // Module Name
    hCore::UIntPtr StartCode; // Start Address
    hCore::UIntPtr StartData; // Allocation Heap
    hCore::UIntPtr StartStack; // Stack Pointer.
    hCore::Int32 Arch; // Architecture and/or platform.
    rt_cookie_type Cookie; // Not shown in public header, this is the way we tell something went wrong.
};

//! @brief Cookie Sanity check.
hCore::Boolean hcore_tls_check(ThreadInformationBlock* ptr);

#include "ThreadLocalStorage.inl"

// last rev 1/27/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
