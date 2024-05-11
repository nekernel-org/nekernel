/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <StorageKit/ATA.hpp>

using namespace NewOS;

/// @brief Class constructor
/// @param Out Disk output
/// @param In  Disk input
/// @param Cleanup Disk cleanup.
ATADeviceInterface::ATADeviceInterface(
	void (*Out)(MountpointInterface* outpacket),
	void (*In)(MountpointInterface* inpacket),
	void (*Cleanup)(void))
	: DeviceInterface(Out, In), fCleanup(Cleanup)
{
}

/// @brief Class desctructor
ATADeviceInterface::~ATADeviceInterface()
{
	MUST_PASS(fCleanup);
	if (fCleanup)
		fCleanup();
}

/// @brief Returns the name of the device interface.
/// @return it's name as a string.
const char* ATADeviceInterface::Name() const
{
	return "ATADeviceInterface";
}

/// @brief Output operator.
/// @param Data
/// @return
ATADeviceInterface& ATADeviceInterface::operator<<(MountpointInterface* Data)
{
	if (!Data)
		return *this;

	for (SizeT driveCount = 0; driveCount < kDriveManagerCount; ++driveCount)
	{
		auto interface = Data->GetAddressOf(driveCount);
		if ((interface) && rt_string_cmp((interface)->fDriveKind(), "ATA-", 5) == 0)
		{
			continue;
		}
		else if ((interface) &&
				 rt_string_cmp((interface)->fDriveKind(), "ATA-", 5) != 0)
		{
			return *this;
		}
	}

	return (ATADeviceInterface&)DeviceInterface<MountpointInterface*>::operator<<(
		Data);
}

/// @brief Input operator.
/// @param Data
/// @return
ATADeviceInterface& ATADeviceInterface::operator>>(MountpointInterface* Data)
{
	if (!Data)
		return *this;

	for (SizeT driveCount = 0; driveCount < kDriveManagerCount; ++driveCount)
	{
		auto interface = Data->GetAddressOf(driveCount);
		if ((interface) && rt_string_cmp((interface)->fDriveKind(), "ATA-", 5) == 0)
		{
			continue;
		}
		else if ((interface) &&
				 rt_string_cmp((interface)->fDriveKind(), "ATA-", 5) != 0)
		{
			return *this;
		}
	}

	return (ATADeviceInterface&)DeviceInterface<MountpointInterface*>::operator>>(
		Data);
}
