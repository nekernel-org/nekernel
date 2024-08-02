/* -------------------------------------------

	Copyright ZKA Technologies

	File: Hart.hxx
	Purpose: RISC-V hardware threads.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

typedef Kernel::Int32 Rv64HartType;

/// @brief Set PC to specific hart.
/// @param hart the hart
/// @param epc the pc.
/// @return
EXTERN_C Kernel::Void hal_set_pc_to_hart(Rv64HartType hart, Kernel::VoidPtr epc);
