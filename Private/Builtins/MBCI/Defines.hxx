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

/// @brief Host interface
struct MBCIHostInterface final {
	UInt32 HostId;
	UInt32 VendorId;
	UInt64 BaseAddressRegister;
	UInt64 DeviceSize;
	UInt8  MemoryType;
	UInt8  HostType;
	UInt8  HostFlags;
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

MBCIHostInterface* drv_get_mbi_host(void);
} // namespace NewOS
