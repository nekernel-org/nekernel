/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/CodeMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/Utils.h>

namespace Kernel {
/***********************************************************************************/
/// @brief Executes a new process from a function. Kernel code only.
/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be
/// accessible.
/// @param main the start of the process.
/// @return if the process was started or not.
/***********************************************************************************/

ProcessID rtl_create_user_process(rtl_main_kind main, const Char* process_name) noexcept {
  if (!process_name || *process_name == 0) return kSchedInvalidPID;

  return UserProcessScheduler::The().Spawn(process_name, reinterpret_cast<VoidPtr>(main), nullptr);
}
}  // namespace Kernel
