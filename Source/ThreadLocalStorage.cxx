/*
 * ========================================================
 *
 * hCore
 * Copyright Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>

#include <KernelKit/SharedObjectCore.hxx>

using namespace hCore;

Boolean hcore_tls_check(VoidPtr ptr)
{
    if (!ptr)
        return false;

    const char *_ptr = (const char *)ptr;
    return _ptr[0] == kRTLMag0 && _ptr[1] == kRTLMag1 && _ptr[2] == kRTLMag2;
}

void hcore_tls_check_syscall_impl(VoidPtr ptr) noexcept
{
    if (!hcore_tls_check(ptr))
    {
        kcout << "TLS: Thread check failure, crashing...\n";
        ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
    }
}
