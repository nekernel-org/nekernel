/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define cPCIConfigAddressPort (0xCF8)
#define cPCIConfigDataPort	  (0xCFC)

#define cPCIDeviceCount (32)
#define cPCIFuncCount	(8)
#define cPCIBusCount	(255)

namespace Kernel::PCI
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
} // namespace Kernel::PCI
