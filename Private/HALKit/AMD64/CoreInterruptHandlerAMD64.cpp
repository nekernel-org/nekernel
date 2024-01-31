/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <NewKit/String.hpp>

extern "C" void idt_handle_system_call(HCore::UIntPtr rsp) {
  HCore::HAL::StackFrame *sf = reinterpret_cast<HCore::HAL::StackFrame *>(rsp);
  rt_syscall_handle(sf);

  HCore::kcout << "System Call with ID: "
               << HCore::StringBuilder::FromInt("syscall{%}", sf->SID);
}

extern "C" void idt_handle_gpf(HCore::UIntPtr rsp) {
  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "General Protection Fault, Caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_scheduler(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "Will be scheduled back later "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  /// schedule another process.
  if (!HCore::ProcessHelper::StartScheduling()) {
    HCore::kcout << "Let's continue schedule this process...\r\n";
  }
}

extern "C" void idt_handle_pf(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Segmentation Fault, Caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_math(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Math error, Caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_generic(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("sp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Processor error, Caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}
