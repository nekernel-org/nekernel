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

  if (sf->IntNum == 0x21) {
    rt_syscall_handle(sf);
  }

  HCore::HAL::Out8(0x20, 0x20);
  HCore::HAL::Out8(0xa0, 0x20);

  HCore::HAL::rt_sti();

  return rsp;
}
