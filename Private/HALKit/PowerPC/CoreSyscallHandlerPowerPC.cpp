/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <NewKit/Array.hpp>

HCore::Array<void (*)(HCore::Int32 id, HCore::HAL::StackFrame *), kMaxSyscalls>
    kSyscalls;

extern "C" void rt_syscall_handle(HCore::HAL::StackFrame *stack) {
  for (HCore::SizeT index = 0UL; index < kMaxSyscalls; ++index) {
    (kSyscalls[index].Leak().Leak())(stack->ID, stack);
  }
}
