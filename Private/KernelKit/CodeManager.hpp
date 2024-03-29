/* -------------------------------------------

    Copyright Mahrouss Logic

    File: CodeManager.hpp
    Purpose: Code Manager and Shared Objects.

    Revision History:

    30/01/24: Added file (amlel)
    3/8/24: Add UPP struct.

------------------------------------------- */

#pragma once

#include <KernelKit/PECodeManager.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/PEFSharedObject.hxx>

#define kUPPNameLen 64

namespace NewOS {
/// @brief Main process entrypoint.
typedef void (*MainKind)(void);

/// @brief Executes a new process from memory.
/// @param main 
/// @param processName 
/// @return 
bool execute_from_image(MainKind main, const char* processName);
} // namespace NewOS