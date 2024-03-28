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
/// \brief Much like Mac OS's UPP.
/// This is read-only by design.
/// It handles different kind of code.
/// ARM <-> AMD64 for example.
typedef struct UniversalProcedureTable final {
  const Char NAME[kUPPNameLen];
  const VoidPtr TRAP;
  const SizeT ARCH;
} PACKED UniversalProcedureTableType;
} // namespace NewOS