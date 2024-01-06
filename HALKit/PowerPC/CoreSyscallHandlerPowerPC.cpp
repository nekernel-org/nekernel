/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <NewKit/Array.hpp>

hCore::Array<void (*)(hCore::Int32 id, hCore::HAL::StackFrame *), kMaxSyscalls> kSyscalls;

extern "C" void rt_syscall_handle(hCore::HAL::StackFrame *stack)
{
    for (hCore::SizeT index = 0UL; index < kMaxSyscalls; ++index)
    {
        (kSyscalls[index].Leak().Leak())(stack->ID, stack);
    }
}
