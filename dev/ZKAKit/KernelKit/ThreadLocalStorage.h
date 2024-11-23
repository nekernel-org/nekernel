/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#ifndef KERNELKIT_TLS_H
#define KERNELKIT_TLS_H

#include <NewKit/Defines.h>

///! @brief Thread Local Storage for minoskrnl.

#define kCookieMag0 'Z'
#define kCookieMag1 'K'
#define kCookieMag2 'A'

#define kTLSCookieLen (3U)

struct THREAD_INFORMATION_BLOCK;

/// @brief Thread Information Block.
/// Located in GS on AMD64, other architectures have their own stuff. (64x0, 32x0, ARM64)
struct PACKED THREAD_INFORMATION_BLOCK final
{
	Kernel::Char	Cookie[kTLSCookieLen]{0}; //! Thread magic number.
	Kernel::VoidPtr Record{nullptr};	//! Thread information record.
};

///! @brief Cookie Sanity check.
Kernel::Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* the_tib);

///! @brief new ptr syscall.
template <typename T>
T* tls_new_ptr(void) noexcept;

///! @brief delete ptr syscall.
template <typename T>
Kernel::Boolean tls_delete_ptr(T* ptr) noexcept;

template <typename T, typename... Args>
T* tls_new_class(Args&&... args);

/// @brief TLS install TIB and PIB. (syscall)
EXTERN_C Kernel::Void rt_install_tib(THREAD_INFORMATION_BLOCK* TIB, THREAD_INFORMATION_BLOCK* PIB);

/// @brief TLS check (syscall)
EXTERN_C Kernel::Bool tls_check_syscall_impl(Kernel::VoidPtr TIB) noexcept;

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 7/7/24

#endif /* ifndef KERNELKIT_TLS_H */
