/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: CodeMgr.h
  Purpose: Code Mgr.

  Revision History:

  30/01/24: Added file (amlel)
  3/8/24: Add UPP struct.

------------------------------------------- */

#pragma once

#include <KernelKit/IPEFDylibObject.h>
#include <KernelKit/PECodeMgr.h>
#include <KernelKit/PEFCodeMgr.h>

namespace Kernel {
/// @brief Main process entrypoint.
typedef void (*rtl_main_kind)(void);

/// @brief C++ Constructor entrypoint.
typedef void (*rtl_cxx_ctor_kind)(void);

/// @brief C++ Destructor entrypoint.
typedef void (*rtl_cxx_dtor_kind)(void);

/// @brief Executes a new process from a function. Kernel code only.
/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be
/// accessible.
/// @param main the start of the process.
/// @return The team's process id.
ProcessID rtl_create_kernel_process(rtl_main_kind main, const Char* task_name) noexcept;

/// @brief Executes a new process from a function. User code only.
/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be
/// accessible.
/// @param main the start of the process.
/// @return The team's process id.
ProcessID rtl_create_user_process(rtl_main_kind main, const Char* process_name) noexcept;
}  // namespace Kernel
