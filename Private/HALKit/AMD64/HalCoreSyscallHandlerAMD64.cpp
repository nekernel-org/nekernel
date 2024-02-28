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

typedef HCore::Void (*rt_syscall_proc)(HCore::Int32 id, HCore::HAL::StackFramePtr);

HCore::Array<rt_syscall_proc, kKernelMaxSystemCalls> kSyscalls;

/// @brief Interrupt system call handler.
extern "C" void rt_syscall_handle(HCore::HAL::StackFramePtr stack) {
  if (stack->R15 < kKernelMaxSystemCalls && kSyscalls[stack->R15] != 0) {
    (kSyscalls[stack->R15].Leak().Leak())(stack->R15, stack);
  }
}
