/* -------------------------------------------

    Copyright SoftwareLabs

    File: Hart.hxx
    Purpose: POWER hardware threads.

    Revision History:

    14/04/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

/// @brief hardware thread indentification type.
typedef NewOS::Int32 PPCHartType;

/// @brief Hardware thread information structure.
typedef struct HalHardwareThread
{
	NewOS::UIntPtr fStartAddress;
	NewOS::UInt8   fPrivleged : 1;
	NewOS::UInt32  fPageFlags;
	PPCHartType	   fIdentNumber;
} HalHardwareThread;
