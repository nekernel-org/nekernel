/* -------------------------------------------

    Copyright Mahrouss Logic

    File: API.hxx
    Purpose: AHCI API.

    Revision History:

    03/17/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <AHCI/Defines.hxx>
#include <NewKit/Defines.hpp>
#include <ObjectKit/Object.hxx>

#define kObjectAHCINamespace "AHCI_DRV\\"

namespace HCore::Builtins {
inline Boolean ke_get_ahci_handle(ObjectPtr* ppAhciObject);
} // namespace HCore::Builtins