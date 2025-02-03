/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: HPET.h
	Purpose: HPET builtin.

	Revision History:

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <Mod/ACPI/ACPI.h>

namespace Kernel
{
	struct PACKED HPETAddressStructure final
	{
		Kernel::UInt8  AddressSpaceId; // 0 - system memory, 1 - system I/O
		Kernel::UInt8  RegisterBitWidth;
		Kernel::UInt8  RegisterBitOffset;
		Kernel::UInt8  Reserved;
		Kernel::UInt64 Address;
	};

	struct PACKED HPETHeader final : public SDT
	{
		Kernel::UInt8		 HardwareRevId;
		Kernel::UInt8		 ComparatorCount : 5;
		Kernel::UInt8		 CounterSize : 1;
		Kernel::UInt8		 Reserved : 1;
		Kernel::UInt8		 LegacyReplacement : 1;
		Kernel::UInt16		 PciVendorId;
		HPETAddressStructure Address;
		Kernel::UInt8		 HpetNumber;
		Kernel::UInt16		 MinimumTick;
		Kernel::UInt8		 PageProtection;
	};

} // namespace Kernel
