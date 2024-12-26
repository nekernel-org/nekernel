/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#ifndef _INC_MODULE_MBCI_H_
#define _INC_MODULE_MBCI_H_

#include <NewKit/Defines.h>
#include <Mod/ACPI/ACPI.h>

/**
- VCC (IN) (OUT for MCU)
- CLK (IN) (OUT for MCU)
- ACK (BI) (Contains an Acknowledge Packet Frame)
- D0- (IN) (Starts with the Host Interface Packet Frame)
- D1- (IN) (Starts with the Host Interface Packet Frame)
- D0+ (OUT) (Starts with the Host Interface Packet Frame)
- D1+ (OUT) (Starts with the Host Interface Packet Frame)
- GND (IN) (OUT for MCU)
 */

#define kMBCIZeroSz (8)

namespace Kernel
{
	struct IMBCIHost;
	struct IMBCIHostPacketFrame;

	/// @brief MBCI Packet frame header
	struct PACKED IMBCIHostPacketFrame final
	{
		UInt32 Magic;
		UInt32 HostId;
		UInt32 Flags;
		UInt32 VendorId;
		UInt32 DeviceId;
		UInt32 DeviceSpeed;
		Bool   Acknowledge;
		Char   Zero[kMBCIZeroSz];
	};

	enum
	{
		kMBCISpeedDeviceInvalid,
		kMBCILowSpeedDevice,
		kMBCIHighSpeedDevice,
		kMBCISpeedDeviceCount,
	};

	/// @brief MBCI Host Interface header.
	struct PACKED IMBCIHost final
	{
		UInt32 Magic;
		UInt32 HostId;
		UInt16 VendorId;
		UInt16 DeviceId;
		UInt8  MemoryType;
		UInt16 HostType;
		UInt16 HostFlags;
		UInt8  Error;
		UInt8  Status;
		UInt8  InterruptEnable;
		UInt64 BaseAddressRegister;
		UInt64 BaseAddressRegisterSize;
		Char   Zero[kMBCIZeroSz];
	};

	/// @brief MBCI host flags.
	enum MBCIHostFlags
	{
		kMBCIHostFlagsSupportsNothing,			 // Invalid MBCI device.
		kMBCIHostFlagsSupportsAPM,				 // Advanced Power Management.
		kMBCIHostFlagsSupportsDaisyChain,		 // Is daisy chained.
		kMBCIHostFlagsSupportsHWInterrupts,		 // Has HW interrupts.
		kMBCIHostFlagsSupportsDMA,				 // Has DMA.
		kMBCIHostFlagsExtended = __UINT16_MAX__, // Extended flags table.
	};

	enum MBCIHostKind
	{
		kMBCIHostKindHardDisk,
		kMBCIHostKindOpticalDisk,
		kMBCIHostKindKeyboardLow,
		kMBCIHostKindMouseLow,
		kMBCIHostKindMouseHigh,
		kMBCIHostKindKeyboardHigh,
		kMBCIHostKindNetworkInterface,
		kMBCIHostKindDaisyChain,
		kMBCIHostKindStartExtended = __UINT16_MAX__, // Extended vendor table limit.
	};

	/// @brief An AuthKey is a context used to decrpy data from an MBCI packet.
	typedef UInt64 MBCIAuthyKeyType;
} // namespace Kernel

#endif // ifndef _INC_MODULE_MBCI_H_
