/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kCookieMag0 'H'
#define kCookieMag1 'C'
#define kCookieMag2 'R'

template <typename T>
T *tls_new_ptr(void);

template <typename T>
bool tls_delete_ptr(T *ptr);

template <typename T, typename... Args>
T *tls_new_class(Args &&...args);

#define kTLSCookieLen 3

/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, Virtual Address 0x10000 (64x0, 32x0, ARM64)
struct PACKED ThreadInformationBlock final {
  NewOS::Char    Cookie[kTLSCookieLen];
  NewOS::UIntPtr StartCode;       // Start Address
  NewOS::UIntPtr StartData;       // Allocation Heap
  NewOS::UIntPtr StartStack;      // Stack Pointer.
  NewOS::Int32   ThreadID;                // Thread execution ID.
};

/// @brief TLS install TIB
EXTERN_C void rt_install_tib(ThreadInformationBlock *pTib, NewOS::VoidPtr pPib);

///! @brief Cookie Sanity check.
NewOS::Boolean tls_check_tib(ThreadInformationBlock* ptr);

/// @brief TLS check system call
EXTERN_C NewOS::Void tls_check_syscall_impl(NewOS::HAL::StackFramePtr stackPtr) noexcept;

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 1/29/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
