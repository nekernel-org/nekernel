/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Interface.hxx
    Purpose: AHCI Interface.

    Revision History:

    03/17/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <AHCI/Defines.hxx>
#include <NewKit/Defines.hpp>
#include <ObjectKit/ObjectKit.hxx>
#include <HintKit/CompilerHint.hxx>

#define kObjectAHCINamespace "AHCI\\"

namespace HCore::Builtins {
/// @brief Returns an AHCI handle.
/// @param pointerAhciObject the handle to pass. 
/// @return 
inline Boolean HcGetHandleAHCI(_InOut ObjectPtr* pointerAhciObject);
} // namespace HCore::Builtins