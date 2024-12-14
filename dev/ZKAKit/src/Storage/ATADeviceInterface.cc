/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/ATA.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
ATADeviceInterface::ATADeviceInterface(
	void (*Out)(MountpointInterface* outpacket),
	void (*In)(MountpointInterface* inpacket),
	void (*Cleanup)(void))
	: IDeviceObject(Out, In), fCleanup(Cleanup)
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
const Char* ATADeviceInterface::Name() const
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

	for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount)
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

	return (ATADeviceInterface&)IDeviceObject<MountpointInterface*>::operator<<(
		Data);
}

/// @brief Input operator.
/// @param Data
/// @return
ATADeviceInterface& ATADeviceInterface::operator>>(MountpointInterface* Data)
{
	if (!Data)
		return *this;

	for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount)
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

	return (ATADeviceInterface&)IDeviceObject<MountpointInterface*>::operator>>(
		Data);
}
