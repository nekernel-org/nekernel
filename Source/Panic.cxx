/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/Panic.hpp>
#include <ArchKit/Arch.hpp>
#include <NewKit/String.hpp>

#include <KernelKit/DebugOutput.hpp>

extern "C" [[noreturn]] void wait_for_debugger()
{
    while (true)
    {
        hCore::HAL::rt_cli();
        hCore::HAL::rt_halt();
    }
}

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace hCore
{
    void panic(const hCore::Int& id)
    {
#ifdef __DEBUG__
        kcout << "hCore: Kernel Panic! \r\n";
        kcout << StringBuilder::FromInt("kError : %\n", id);

        DumpManager::Dump();
#endif // __DEBUG__

        wait_for_debugger();
    }

    void runtime_check(bool expr, const char *file, const char *line)
    {
        if (!expr)
        {
#ifdef __DEBUG__
            kcout << "[KERNEL] Check Failed!\n";
            kcout << "[KERNEL] File: " << file << "\n";
            kcout << "[KERNEL] Where: " << line << "\n";

#endif // __DEBUG__

            hCore::panic(RUNTIME_CHECK_FAILED); // Runtime Check failed
        }
    }
} // namespace hCore
