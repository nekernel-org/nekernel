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

typedef HCore::Void (*rt_syscall_proc)(HCore::HAL::StackFramePtr);

HCore::Array<rt_syscall_proc, kKernelMaxSystemCalls> kSyscalls;

/// @brief Interrupt system call handler.
EXTERN_C void rt_syscall_handle(HCore::HAL::StackFramePtr stack) {
  if (!stack) return;

  if (stack->Rcx < kKernelMaxSystemCalls && kSyscalls[stack->Rcx] != 0) {
    (kSyscalls[stack->Rcx].Leak().Leak())(stack);
  }
}
