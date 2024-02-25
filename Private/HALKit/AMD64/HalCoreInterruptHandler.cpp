/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <NewKit/String.hpp>

extern "C" HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr &rsp) {
  HCore::HAL::rt_cli();

  HCore::HAL::StackFramePtr sf = (HCore::HAL::StackFramePtr)rsp;

  rt_syscall_handle(sf);

  HCore::kcout << "Krnl\\rt_handle_interrupts: Done\r\n";

  HCore::HAL::rt_sti();
  return rsp;
}
