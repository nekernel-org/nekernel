/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <Modules/ACPI/ACPI.hxx>

/**
- VCC (IN) (OUT for MCU)
- CLK (IN) (OUT for MCU)
- ACK (BI) (Contains an Acknowledge Packet Frame)
- D0- (IN) (Starts with the Host Imterface Packet Frame)
- D1- (IN) (Starts with the Host Imterface Packet Frame)
- D0+ (OUT) (Starts with the Host Imterface Packet Frame)
- D1+ (OUT) (Starts with the Host Imterface Packet Frame)
- GND (IN) (OUT for MCU)
 */

#define cMBCIZeroSz (8)
#define cMBCIMagic "MBCI  "

namespace Kernel
{
	struct MBCIHostInterface;
	struct MBCIPacketACK;

	/// @brief MBCI Acknowledge header.
	struct PACKED MBCIPacketACK final
	{
		UInt32 Magic;
		UInt32 HostId;
		UInt16 VendorId;
		UInt16 DeviceId;
		Bool   Acknowleged;
		Char   Zero[cMBCIZeroSz];
	};

	/// @brief MBCI Host Interface header.
	struct PACKED MBCIHostInterface final
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
		Char   Zero[cMBCIZeroSz];
	};

	/// @brief MBCI host flags.
	enum MBCIHostFlags
	{
		kMBCIHostFlagsSupportsPageProtection,	 /// Page protected.
		kMBCIHostFlagsSupportsAPM,				 /// Advanced Power Management.
		kMBCIHostFlagsSupportsDaisyChain,		 /// Is daisy chained.
		kMBCIHostFlagsSupportsHWInterrupts,		 /// Has HW interrupts.
		kMBCIHostFlagsSupportsDMA,				 /// Has DMA.
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
		kMBCIHostKindStartExtended = __UINT16_MAX__, /// Extended vendor table.
	};
} // namespace Kernel
