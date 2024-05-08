/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */
#pragma once

#include <NewKit/Defines.hpp>

#define PCI_CONFIG_ADDRESS (0xCF8)
#define PCI_CONFIG_DATA	   (0xCFC)

#define PCI_DEVICE_COUNT (32)
#define PCI_FUNC_COUNT	 (8)
#define PCI_BUS_COUNT	 (255)

namespace NewOS::PCI
{
	// model
	struct DeviceHeader
	{
		UInt16 VendorId;
		UInt16 DeviceId;
		UInt8  Command;
		UInt8  Status;
		UInt8  RevisionId;
		UInt8  ProgIf;
		UInt8  SubClass;
		UInt8  Class;
		UInt8  CacheLineSz;
		UInt8  LatencyTimer;
		UInt8  HeaderType;
		UInt8  Bist;
		UInt8  Bus;
		UInt8  Device;
		UInt8  Function;
	};

	namespace Detail
	{
		class BAR
		{
		public:
			UIntPtr BAR;
			SizeT	Size;
		};
	} // namespace Detail

	class BAR
	{
	public:
		Detail::BAR BAR1;
		Detail::BAR BAR2;
		Detail::BAR BAR3;
		Detail::BAR BAR4;
		Detail::BAR BAR5;
	};
} // namespace NewOS::PCI
