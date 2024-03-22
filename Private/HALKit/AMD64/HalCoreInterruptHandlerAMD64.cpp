/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hpp>
#include <NewKit/String.hpp>

EXTERN_C void idt_handle_gpf(HCore::UIntPtr rsp) {
  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout << "NewKernel.exe: Stack Pointer: " << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "NewKernel.exe: General Protection Fault, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_scheduler(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  HCore::kcout
      << "NewKernel.exe: Will be scheduled back later "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName()
      << HCore::end_line();

  /// schedule another process.
  if (!HCore::ProcessHelper::StartScheduling()) {
    HCore::kcout << "NewKernel.exe: Continue schedule this process...\r\n";
  }
}

EXTERN_C void idt_handle_pf(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "NewKernel.exe: Segmentation Fault, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_math(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "NewKernel.exe: Math error, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_generic(HCore::UIntPtr rsp) {
  HCore::kcout << HCore::StringBuilder::FromInt("sp{%}", rsp);

  MUST_PASS(HCore::ProcessManager::Shared().Leak().GetCurrent());

  HCore::kcout
      << "NewKernel.exe: Execution error, caused by "
      << HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().GetName();

  HCore::ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
}
