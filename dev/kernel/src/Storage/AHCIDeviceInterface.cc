/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/AHCI.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
AHCIDeviceInterface::AHCIDeviceInterface(void (*out)(IDeviceObject* self, MountpointInterface* outpacket),
										 void (*in)(IDeviceObject* self, MountpointInterface* inpacket),
										 void (*cleanup)(void))
	: IDeviceObject(out, in), fCleanup(cleanup)
{
}

/// @brief Class desctructor
AHCIDeviceInterface::~AHCIDeviceInterface()
{
	MUST_PASS(fCleanup);

	if (fCleanup)
		fCleanup();
}

/// @brief Returns the name of the device interface.
/// @return it's name as a string.
const Char* AHCIDeviceInterface::Name() const
{
	return "/dev/sda{}";
}
