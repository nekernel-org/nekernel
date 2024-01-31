/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <HALKit/AMD64/Processor.hpp>
#include <KernelKit/PermissionSelector.hxx>

HCore::Array<void (*)(HCore::Int32 id, HCore::HAL::StackFrame *), kMaxSyscalls>
    kSyscalls;

// IDT System Call Handler.
// NOTE: don't trust the user.
extern "C" void rt_syscall_handle(HCore::HAL::StackFrame *stack) {
  for (HCore::SizeT index = 0UL; index < kMaxSyscalls; ++index) {
    (kSyscalls[index].Leak().Leak())(stack->SID, stack);
  }
}
