/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/AMD64/Processor.hpp>
#include <KernelKit/PermissionSelector.hxx>

HCore::Array<void (*)(HCore::Int32 id, HCore::HAL::StackFrame *), kMaxSyscalls>
    kSyscalls;

/// @brief Interrupt system call handler.
extern "C" void rt_syscall_handle(HCore::HAL::StackFrame *stack) {
  for (HCore::SizeT index = 0UL; index < kMaxSyscalls; ++index) {
    if (kSyscalls[index]) (kSyscalls[index].Leak().Leak())(stack->R15, stack);
  }
}
