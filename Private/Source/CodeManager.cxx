/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>
#include <KernelKit/ProcessScheduler.hpp>

using namespace NewOS;

/// @brief Executes a new process from a function. kernel code only.
/// @param main the start of the process.
/// @return
bool execute_from_image(MainKind main, const char* processName) noexcept {
  if (!main) return false;

  ProcessHeader proc((VoidPtr)main);
  proc.Kind = ProcessHeader::kDriverKind;
  rt_copy_memory((VoidPtr)processName, proc.Name, rt_string_len(proc.Name));

  Ref<ProcessHeader> refProc = proc;

  return ProcessScheduler::Shared().Leak().Add(refProc);
}