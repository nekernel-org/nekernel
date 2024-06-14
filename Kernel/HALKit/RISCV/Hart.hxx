/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: Hart.hxx
	Purpose: RISC-V hardware threads.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

typedef NewOS::Int32 Rv64HartType;

/// @brief Set PC to specific hart.
/// @param hart the hart
/// @param epc the pc.
/// @return 
EXTERN_C NewOS::Void hal_switch_to_hart(Rv64HartType hart, NewOS::VoidPtr epc);