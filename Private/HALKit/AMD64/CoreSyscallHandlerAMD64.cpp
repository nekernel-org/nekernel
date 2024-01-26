/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/PermissionSelector.hxx>
#include <HALKit/AMD64/Processor.hpp>

#include <ArchKit/Arch.hpp>

hCore::Array<void (*)(hCore::Int32 id, hCore::HAL::StackFrame *), kMaxSyscalls> kSyscalls;

// IDT System Call Handler.
// NOTE: don't trust the user.
extern "C" void rt_syscall_handle(hCore::HAL::StackFrame *stack)
{
    for (hCore::SizeT index = 0UL; index < kMaxSyscalls; ++index)
    {
        (kSyscalls[index].Leak().Leak())(stack->SID, stack);
    }
}
