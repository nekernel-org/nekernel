/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Firmware::Detail::CoreBoot
{
	using namespace Kernel;

	struct COREBOOT_LINEAR_EXEC;

	/// @brief Linear Executable Header
	/// @author Amlal El Mahrouss
	struct ATTRIBUTE(aligned(4)) COREBOOT_LINEAR_EXEC
	{
		const Char	 fMagic[2];		// magic number
		const Char	 fName[10];		// operating system name
		const UInt32 fRevision;		// firmware revision
		const UInt32 fStartAddress; // start address (master/slave(s) thread)

#ifdef NE_IS_EXTENDED_COREBOOT
		const UIntPtr fMasterStructure;		   // master structure for MP/PM and device tree and such. (ARM)
		const UIntPtr fMasterStructureVersion; // master structure version.
#endif

#ifdef NE_IS_MBCI_COREBOOT
		const UIntPtr fMBCIStructure;		 // MBCI structure for MBCI (ARM)
		const UIntPtr fMBCIStructureVersion; // MBCI structure version.
#endif
	};
} // namespace Firmware::Detail::CoreBoot
