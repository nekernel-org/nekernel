/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hpp>
#include <NewKit/String.hpp>

/// @brief Handle GPF fault.
/// @param rsp 
EXTERN_C void idt_handle_gpf(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout << "NewKernel.exe: Stack Pointer: "
               << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: General Protection Fault, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle the scheduler interrupt, raised from the HPET timer.
/// @param rsp 
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

/// @brief Handle page fault.
/// @param rsp 
EXTERN_C void idt_handle_pf(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());
  NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: Segmentation Fault, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle math fault.
/// @param rsp 
EXTERN_C void idt_handle_math(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());
  NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: Math error, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp 
EXTERN_C void idt_handle_generic(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());
  NewOS::kcout << NewOS::StringBuilder::FromInt("sp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: Execution error, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle #UD fault.
/// @param rsp 
EXTERN_C void idt_handle_ud(NewOS::UIntPtr rsp) {
  MUST_PASS(NewOS::ProcessScheduler::Shared().Leak().GetCurrent());

  NewOS::kcout << "NewKernel.exe: Stack Pointer: "
               << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

  NewOS::kcout
      << "NewKernel.exe: Invalid interrupt, caused by "
      << NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().GetName();

  NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
}