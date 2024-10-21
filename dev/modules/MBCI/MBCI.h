/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef _INC_MODULE_MBCI_HXX_
#define _INC_MODULE_MBCI_HXX_

#include <NewKit/Defines.h>
#include <modules/ACPI/ACPI.h>

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

#define cMBCIZeroSz (8)
#define cMBCIMagic	"MBCI  "

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
		Char   Zero[cMBCIZeroSz];
	};

	enum
	{
		eMBCISpeedDeviceInvalid,
		eMBCILowSpeedDevice,
		eMBCIHighSpeedDevice,
		eMBCISpeedDeviceCount,
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
		Char   Zero[cMBCIZeroSz];
	};

	/// @brief MBCI host flags.
	enum MBCIHostFlags
	{
		eMBCIHostFlagsSupportsNothing,			 // Invalid MBCI device.
		eMBCIHostFlagsSupportsAPM,				 // Advanced Power Management.
		eMBCIHostFlagsSupportsDaisyChain,		 // Is daisy chained.
		eMBCIHostFlagsSupportsHWInterrupts,		 // Has HW interrupts.
		eMBCIHostFlagsSupportsDMA,				 // Has DMA.
		eMBCIHostFlagsExtended = __UINT16_MAX__, // Extended flags table.
	};

	enum MBCIHostKind
	{
		eMBCIHostKindHardDisk,
		eMBCIHostKindOpticalDisk,
		eMBCIHostKindKeyboardLow,
		eMBCIHostKindMouseLow,
		eMBCIHostKindMouseHigh,
		eMBCIHostKindKeyboardHigh,
		eMBCIHostKindNetworkInterface,
		eMBCIHostKindDaisyChain,
		eMBCIHostKindStartExtended = __UINT16_MAX__, // Extended vendor table.
	};

	/// @brief An AuthKey is a context used to decrpy data from an MBCI packet.
	typedef UInt64 MBCIAuthyKeyType;
} // namespace Kernel

#endif // ifndef _INC_MODULE_MBCI_HXX_
