/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <Builtins/ACPI/ACPI.hxx>

namespace NewOS
{
	struct MBCIHostInterface;

	/// @brief MBCI Host Interface header.
	struct PACKED MBCIHostInterface final
	{
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
} // namespace NewOS
