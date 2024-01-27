/*
 * ========================================================
 *
 * hCore
 * Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>

using namespace hCore;

Boolean hcore_tls_check(VoidPtr ptr)
{
    if (!ptr)
        return false;

    const char *_ptr = (const char *)ptr;
    return _ptr[0] == kRTLMag0 && _ptr[1] == kRTLMag1 && _ptr[2] == kRTLMag2;
}

Void hcore_tls_check_syscall_impl(ThreadInformationBlock ptr) noexcept
{
    if (!hcore_tls_check(ptr.Cookie))
    {
        kcout << "TLS: Verification failure, crashing...\n";
        ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
    }
}
