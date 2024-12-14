/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: AP.h
	Purpose: POWER hardware threads.

	Revision History:

	14/04/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	struct HAL_HARDWARE_THREAD;

	/// @brief hardware thread indentification type.
	typedef Kernel::Int32 hal_ap_kind;

	/// @brief Hardware thread information structure.
	typedef struct HAL_HARDWARE_THREAD
	{
		Kernel::UIntPtr fStartAddress;
		Kernel::UInt8	fPrivleged : 1;
		Kernel::UInt32	fPageMemoryFlags;
		hal_ap_kind		fIdentNumber;
	} HAL_HARDWARE_THREAD;

	/// @brief Set PC to specific hart.
	/// @param hart the hart
	/// @param epc the pc.
	/// @return
	EXTERN_C Kernel::Void hal_set_pc_to_hart(HAL_HARDWARE_THREAD* hart, Kernel::VoidPtr epc);
} // namespace Kernel
