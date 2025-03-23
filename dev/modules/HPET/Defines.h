/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: HPET.h
	Purpose: HPET builtin.

	Revision History:

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <modules/ACPI/ACPI.h>

namespace NeOS
{
	struct PACKED HPETAddressStructure final
	{
		NeOS::UInt8	 AddressSpaceId; // 0 - system memory, 1 - system I/O
		NeOS::UInt8	 RegisterBitWidth;
		NeOS::UInt8	 RegisterBitOffset;
		NeOS::UInt8	 Reserved;
		NeOS::UInt64 Address;
	};

	struct PACKED HPETHeader final : public SDT
	{
		NeOS::UInt8			 HardwareRevId;
		NeOS::UInt8			 ComparatorCount : 5;
		NeOS::UInt8			 CounterSize : 1;
		NeOS::UInt8			 Reserved : 1;
		NeOS::UInt8			 LegacyReplacement : 1;
		NeOS::UInt16		 PciVendorId;
		HPETAddressStructure Address;
		NeOS::UInt8			 HpetNumber;
		NeOS::UInt16		 MinimumTick;
		NeOS::UInt8			 PageProtection;
	};

} // namespace NeOS
