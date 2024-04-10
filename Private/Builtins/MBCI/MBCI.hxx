/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <Builtins/ACPI/ACPI.hxx>

namespace NewOS {
struct MBCIHostInterface;
struct MBCIDeviceInterface;
struct MBCIPacketInterface;

/// @brief MBCI Host Interface header.
struct MBCIHostInterface final {
	UInt32 HostId;
	UInt16 VendorId;
	UInt16 DeviceId;
	UInt8  MemoryType;
	UInt8  HostType;
	UInt8  HostFlags;
	UInt8  Error;
	UInt8  Status;
	UInt8  InterruptEnable;
	UInt64 BaseAddressRegister;
	UInt64 BaseAddressRegisterSize;
};

/// @brief MBCI host flags.
enum MBCIHostFlags {
	kMBCIHostFlagsSupportsPageProtection, /// Page protected.
	kMBCIHostFlagsSupportsAPM, /// Advanced Power Management.
	kMBCIHostFlagsSupportsDaisyChain, /// Is daisy chained.
	kMBCIHostFlagsSupportsHWInterrupts, /// Has HW interrupts.
	kMBCIHostFlagsSupportsDMA, /// Has DMA.
	kMBCIHostFlagsCount,
};
} // namespace NewOS
