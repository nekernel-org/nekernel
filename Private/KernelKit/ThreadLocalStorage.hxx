/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kCookieMag0 'h'
#define kCookieMag1 'C'
#define kCookieMag2 'o'

template <typename T>
T *hcore_tls_new_ptr(void);

template <typename T>
bool hcore_tls_delete_ptr(T *ptr);

template <typename T, typename... Args>
T *hcore_tls_new_class(Args &&...args);

typedef HCore::Char rt_cookie_type[3];

/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, Virtual Address 0x10000 (64x0, 32x0, ARM64)
struct ThreadInformationBlock final {
  HCore::Char Name[kNameLen];  // Module Name
  HCore::UIntPtr StartCode;    // Start Address
  HCore::UIntPtr StartData;    // Allocation Heap
  HCore::UIntPtr StartStack;   // Stack Pointer.
  HCore::Int32 Arch;           // Architecture and/or platform.
  rt_cookie_type Cookie;  // Not shown in public header, this is the way we tell
                          // something went wrong.
};

/// @brief TLS install TIB
extern void rt_install_tib(ThreadInformationBlock *pTib);

///! @brief Cookie Sanity check.
HCore::Boolean hcore_tls_check(ThreadInformationBlock *ptr);

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 1/29/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
