/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <StorageKit/AHCI.hpp>

using namespace NewOS;

/// @brief Class constructor
/// @param Out Disk output
/// @param In  Disk input
/// @param Cleanup Disk cleanup.
AHCIDeviceInterface::AHCIDeviceInterface(void (*Out)(MountpointInterface* outpacket),
										 void (*In)(MountpointInterface* inpacket),
										 void (*Cleanup)(void))
	: DeviceInterface(Out, In), fCleanup(Cleanup)
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
const char* AHCIDeviceInterface::Name() const
{
	return "AHCIDeviceInterface";
}
