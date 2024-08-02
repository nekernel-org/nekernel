/* -------------------------------------------

	Copyright ZKA Technologies

	File: Hart.hxx
	Purpose: POWER hardware threads.

	Revision History:

	14/04/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

/// @brief hardware thread indentification type.
typedef Kernel::Int32 PPCHartType;

/// @brief Hardware thread information structure.
typedef struct HalHardwareThread
{
	Kernel::UIntPtr fStartAddress;
	Kernel::UInt8	fPrivleged : 1;
	Kernel::UInt32	fPageFlags;
	PPCHartType		fIdentNumber;
} HalHardwareThread;

/// @brief Set PC to specific hart.
/// @param hart the hart
/// @param epc the pc.
/// @return
EXTERN_C Kernel::Void hal_set_pc_to_hart(HalHardwareThread* hart, Kernel::VoidPtr epc);
