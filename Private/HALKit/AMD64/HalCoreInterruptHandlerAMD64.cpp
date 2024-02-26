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

#include "KernelKit/DebugOutput.hpp"

extern "C" void idt_handle_system_call(HCore::UIntPtr rsp) {
  HCore::HAL::StackFrame *sf = reinterpret_cast<HCore::HAL::StackFrame *>(rsp);
  rt_syscall_handle(sf);

  HCore::kcout << "System Call with ID: "
               << HCore::StringBuilder::FromInt("syscall{%}", sf->R15);
}

extern "C" void idt_handle_gpf(HCore::UIntPtr rsp) {
  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "General Protection Fault, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_scheduler(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "HCoreKrnl: Will be scheduled back later "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName()
      << HCore::EndLine();

  /// schedule another process.
  if (!HCore::ProcessHelper::StartScheduling()) {
    HCore::kcout << "HCoreKrnl: Continue schedule this process...\r\n";
  }
}

extern "C" void idt_handle_pf(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Segmentation Fault, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_math(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Math error, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

extern "C" void idt_handle_generic(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("sp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "Execution error, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}
