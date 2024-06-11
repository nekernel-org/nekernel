/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: HPET.hxx
	Purpose: HPET builtin.

	Revision History:

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <Builtins/ACPI/ACPI.hxx>

namespace NewOS
{
	struct PACKED HPETAddressStructure final
	{
		NewOS::UInt8  AddressSpaceId; // 0 - system memory, 1 - system I/O
		NewOS::UInt8  RegisterBitWidth;
		NewOS::UInt8  RegisterBitOffset;
		NewOS::UInt8  Reserved;
		NewOS::UInt64 Address;
	};

	struct PACKED HPETHeader final : public SDT
	{
		NewOS::UInt8		 HardwareRevId;
		NewOS::UInt8		 ComparatorCount : 5;
		NewOS::UInt8		 CounterSize : 1;
		NewOS::UInt8		 Reserved : 1;
		NewOS::UInt8		 LegacyReplacement : 1;
		NewOS::UInt16		 PciVendorId;
		HPETAddressStructure Address;
		NewOS::UInt8		 HpetNumber;
		NewOS::UInt16		 MinimumTick;
		NewOS::UInt8		 PageProtection;
	};

} // namespace NewOS