/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <NewKit/String.hpp>
#include <KernelKit/ProcessManager.hpp>

extern "C" void idt_handle_system_call(hCore::UIntPtr rsp)
{
    hCore::HAL::StackFrame *sf = reinterpret_cast<hCore::HAL::StackFrame*>(rsp);
    rt_syscall_handle(sf);

    hCore::kcout << "System Call with ID: " << hCore::StringBuilder::FromInt("syscall{%}", sf->SID);
}

extern "C" void idt_handle_gpf(hCore::UIntPtr rsp)
{
    MUST_PASS(hCore::ProcessManager::Shared().Leak().GetCurrent());

    hCore::kcout << hCore::StringBuilder::FromInt("sp{%}", rsp);

    hCore::kcout << "General Protection Fault, Caused by "
                   << hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

    hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_scheduler(hCore::UIntPtr rsp)
{
    hCore::kcout << hCore::StringBuilder::FromInt("sp{%}", rsp);

    hCore::kcout << "Will be scheduled back later "
                   << hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

    /// schedule another process.
    if (!hCore::ProcessHelper::StartScheduling())
    {
        hCore::kcout << "Let's continue schedule this process...\r\n";
    }
}

extern "C" void idt_handle_pf(hCore::UIntPtr rsp)
{
    hCore::kcout << hCore::StringBuilder::FromInt("sp{%}", rsp);

    MUST_PASS(hCore::ProcessManager::Shared().Leak().GetCurrent());

    hCore::kcout << "Segmentation Fault, Caused by "
                   << hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

    hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_math(hCore::UIntPtr rsp)
{
    hCore::kcout << hCore::StringBuilder::FromInt("sp{%}", rsp);

    MUST_PASS(hCore::ProcessManager::Shared().Leak().GetCurrent());

    hCore::kcout << "Math error, Caused by "
                   << hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

    hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_generic(hCore::UIntPtr rsp)
{
    hCore::kcout << hCore::StringBuilder::FromInt("sp{%}", rsp);

    MUST_PASS(hCore::ProcessManager::Shared().Leak().GetCurrent());

    hCore::kcout << "Processor error, Caused by "
                   << hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

    hCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}
