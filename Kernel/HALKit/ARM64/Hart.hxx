/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

/// @file HALKit\ARM64\Hart.hxx
/// @brief ARM64 hardware thread concept.

typedef NewOS::Int32 Arm64HartType;

/// @brief Set PC to specific hart.
/// @param hart the hart
/// @param epc the pc.
/// @return 
EXTERN_C NewOS::Void hal_switch_to_hart(Arm64HartType hart, NewOS::VoidPtr epc);

/// @brief Hart IPI enum
enum {
	cHartIpiTurnOn,
	cHartIpiTurnOff,
	cHartIpiStop,
	cHartIpiResume,
};
