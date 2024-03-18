/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kCookieMag0 'h'
#define kCookieMag1 'C'
#define kCookieMag2 'o'

template <typename T>
T *tls_new_ptr(void);

template <typename T>
bool tls_delete_ptr(T *ptr);

template <typename T, typename... Args>
T *tls_new_class(Args &&...args);

#define kTLSCookieLen 3

typedef HCore::Char* rt_cookie_type;

#define kTIBNameLen 256

/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, Virtual Address 0x10000 (64x0, 32x0, ARM64)
struct ThreadInformationBlock final {
  HCore::Char Name[kTIBNameLen];  // Module Name
  HCore::UIntPtr StartCode;       // Start Address
  HCore::UIntPtr StartData;       // Allocation Heap
  HCore::UIntPtr StartStack;      // Stack Pointer.
  HCore::Int32 Arch;              // Architecture and/or platform.
  HCore::Int32 ID;                // Thread execution ID.
  rt_cookie_type Cookie;  // Not shown in public header, location of the cookie header is store here, this is the way we tell
                          // something went wrong.
};

/// @brief TLS install TIB
EXTERN_C void rt_install_tib(ThreadInformationBlock *pTib, HCore::VoidPtr pPib);

///! @brief Cookie Sanity check.
HCore::Boolean tls_check_tib(ThreadInformationBlock *ptr);

/// @brief TLS check system call
EXTERN_C HCore::Void tls_check_syscall_impl(HCore::HAL::StackFramePtr stackPtr) noexcept;

#include <KernelKit/ThreadLocalStorage.inl>

// last rev 1/29/24

#endif /* ifndef _KERNELKIT_TLS_HPP */
