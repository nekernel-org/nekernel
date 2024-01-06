/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _KERNELKIT_TLS_HPP
#define _KERNELKIT_TLS_HPP

#include <NewKit/Defines.hpp>

//! @brief TLS implementation in C++

#define kRTLMag0 'V'
#define kRTLMag1 'C'
#define kRTLMag2 'S'

template <typename T>
T* hcore_tls_new_ptr(void);

template <typename T>
bool hcore_tls_delete_ptr(T* ptr);

template <typename T, typename... Args>
T* hcore_tls_new_class(Args&&... args);

//! @brief Cookie Sanity check.
hCore::Boolean hcore_tls_check(hCore::VoidPtr ptr);

typedef char rt_cookie_type[3];

#include "ThreadLocalStorage.inl"

#endif /* ifndef _KERNELKIT_TLS_HPP */
