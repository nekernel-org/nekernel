/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/AHCI.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
AHCIDeviceInterface::AHCIDeviceInterface(void (*Out)(MountpointInterface* outpacket),
										 void (*In)(MountpointInterface* inpacket),
										 void (*Cleanup)(void))
	: IDeviceObject(Out, In), fCleanup(Cleanup)
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
	return "AHCIDeviceInterface";
}
