/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hpp>
#include <NewKit/String.hpp>

EXTERN_C void idt_handle_gpf(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout << "NewKernel.exe: Stack Pointer: "
               << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: General Protection Fault, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_scheduler(NewOS::UIntPtr rsp) {
  NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: Will be scheduled back later "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName()
      << NewOS::end_line();

  /// schedule another process.
  if (!NewOS::ProcessHelper::StartScheduling()) {
    NewOS::kcout << "NewKernel.exe: Continue schedule this process...\r\n";
  }
}

EXTERN_C void idt_handle_pf(NewOS::UIntPtr rsp) {
  NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout
      << "NewKernel.exe: Segmentation Fault, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_math(NewOS::UIntPtr rsp) {
  NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout
      << "NewKernel.exe: Math error, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

EXTERN_C void idt_handle_generic(NewOS::UIntPtr rsp) {
  NewOS::kcout << NewOS::StringBuilder::FromInt("sp{%}", rsp);

  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout
      << "NewKernel.exe: Execution error, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}
