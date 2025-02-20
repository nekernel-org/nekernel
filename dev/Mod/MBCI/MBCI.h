/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

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

namespace NeOS
{
	struct IMBCIHost;

	enum
	{
		kMBCISpeedDeviceInvalid,
		kMBCILowSpeedDevice,
		kMBCIHighSpeedDevice,
		kMBCISpeedDeviceCount,
	};

	/// @brief MBCI Host header.
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
		UInt32 MMIOTest;
		UInt16 State;
		UInt8  Status;
		UInt8  InterruptEnable;
		UInt64 BaseAddressRegister;
		UInt64 BaseAddressRegisterSize;
		UInt32 CommandIssue;
		Char   Zero[kMBCIZeroSz];
	};

	/// @brief MBCI host flags.
	enum MBCIHostFlags
	{
		kMBCIHostFlagsSupportsNothing,		// Invalid MBCI device.
		kMBCIHostFlagsSupportsAPM,			// Advanced Power Management.
		kMBCIHostFlagsSupportsDaisyChain,	// Is daisy chained.
		kMBCIHostFlagsSupportsHWInterrupts, // Has HW interrupts.
		kMBCIHostFlagsSupportsDMA,			// Has DMA.
		kMBCIHostFlagsExtended,				// Extended flags table.
	};

	/// @brief MBCI host kind.
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
		kMBCIHostKindStartExtended, // Extended vendor table limit.
	};

	enum MBCIHostState
	{
		kMBCIHostStateInvalid,
		kMBCIHostStateReset,
		kMBCIHostStateSuccess,
		kMBCIHostStateReady,
		kMBCIHostStateDmaStart,
		kMBCIHostStateDmaEnd,
		kMBCIHostStateFail,
		kMBCIHostStateCount,
	};

	/// @brief An AuthKey is a context used to decrpy data from an MBCI packet.
	typedef UInt64 MBCIAuthKeyType;
} // namespace NeOS

#endif // ifndef _INC_MODULE_MBCI_H_
