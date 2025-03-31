/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <KernelKit/PCI/Device.h>
#include <NewKit/Defines.h>

namespace Kernel
{
	namespace Types
	{
		// https://wiki.osdev.org/PCI
		enum class PciDeviceKind : UChar
		{
			MassStorageController			  = 0x1,
			NetworkController				  = 0x2,
			DisplayController				  = 0x3,
			MultimediaController			  = 0x4,
			MemoryController				  = 0x5,
			Bridge							  = 0x6,
			CommunicationController			  = 0x7,
			GenericSystemPeripheral			  = 0x8,
			InputDeviceController			  = 0x9,
			DockingStation					  = 0xa,
			Processor						  = 0xb,
			SerialBusController				  = 0xc,
			WirelessController				  = 0xd,
			IntelligentController			  = 0xe,
			SatelliteCommunicationsController = 0xf,
			CoProcessor						  = 0x40,
			Unassgined						  = 0xf,
			Invalid							  = Unassgined,
		};
	} // namespace Types
} // namespace Kernel

inline BOOL operator!=(const Kernel::Types::PciDeviceKind& lhs, Kernel::UChar rhs)
{
	return rhs != (Kernel::UChar)lhs;
}

inline BOOL operator==(const Kernel::Types::PciDeviceKind& lhs, Kernel::UChar rhs)
{
	return rhs == (Kernel::UChar)lhs;
}

inline BOOL operator!=(Kernel::UChar lhs, const Kernel::Types::PciDeviceKind& rhs)
{
	return lhs != (Kernel::UChar)rhs;
}

inline BOOL operator==(Kernel::UChar lhs, const Kernel::Types::PciDeviceKind& rhs)
{
	return lhs == (Kernel::UChar)rhs;
}