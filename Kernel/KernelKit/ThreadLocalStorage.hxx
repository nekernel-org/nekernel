/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kCookieMag0 'H'
#define kCookieMag1 'C'
#define kCookieMag2 'R'

#define kTLSCookieLen (3U)

struct THREAD_INFORMATION_BLOCK;

/// @brief Thread Information Block.
/// Located in GS on AMD64, other architectures have their own stuff. (64x0, 32x0, ARM64)
struct PACKED THREAD_INFORMATION_BLOCK final
{
	Kernel::Char	f_Cookie[kTLSCookieLen]; // Process cookie.
	Kernel::UIntPtr f_Code;			   // Start Address
	Kernel::UIntPtr f_Data;			   // Allocation Heap
	Kernel::UIntPtr f_BSS;			   // Stack Pointer.
	Kernel::Int32	f_ID;			   // Thread execution ID.
};

///! @brief Cookie Sanity check.
Kernel::Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* the_tib);

///! @brief new ptr syscall.
template <typename T>
T* tls_new_ptr(void);

///! @brief delete ptr syscall.
template <typename T>
Kernel::Boolean tls_delete_ptr(T* ptr);

template <typename T, typename... Args>
T* tls_new_class(Args&&... args);

/// @brief TLS install TIB and PIB. (syscall)
EXTERN_C void rt_install_tib(THREAD_INFORMATION_BLOCK* TIB, THREAD_INFORMATION_BLOCK* PIB);

/// @brief TLS check (syscall)
EXTERN_C Kernel::Bool tls_check_syscall_impl(Kernel::VoidPtr TIB) noexcept;

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 7/7/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
