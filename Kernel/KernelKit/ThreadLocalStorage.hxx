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

/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, other architectures have their own stuff. (64x0, 32x0, ARM64)
struct PACKED ThreadInformationBlock final
{
	Kernel::Char	Cookie[kTLSCookieLen]; // Process cookie.
	Kernel::UIntPtr StartCode;			   // Start Address
	Kernel::UIntPtr StartData;			   // Allocation Heap
	Kernel::UIntPtr StartStack;			   // Stack Pointer.
	Kernel::Int32	ThreadID;			   // Thread execution ID.
};

typedef struct ThreadInformationBlock ProcessInformationBlock;

///! @brief Cookie Sanity check.
Kernel::Boolean tls_check_tib(ThreadInformationBlock* Ptr);

///! @brief new ptr syscall.
template <typename T>
T* tls_new_ptr(void);

///! @brief delete ptr syscall.
template <typename T>
Kernel::Boolean tls_delete_ptr(T* ptr);

template <typename T, typename... Args>
T* tls_new_class(Args&&... args);

/// @brief TLS install TIB and PIB. (syscall)
EXTERN_C void rt_install_tib(ThreadInformationBlock* TIB, ThreadInformationBlock* PIB);

/// @brief TLS check (syscall)
EXTERN_C Kernel::Void tls_check_syscall_impl(Kernel::VoidPtr TIB) noexcept;

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 7/7/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
