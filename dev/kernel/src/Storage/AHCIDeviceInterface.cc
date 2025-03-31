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

/// @brief Output operator.
/// @param Data the disk mountpoint.
/// @return the class itself after operation.
AHCIDeviceInterface& AHCIDeviceInterface::operator<<(MountpointInterface* Data)
{
	if (!Data)
		return *this;

	for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount)
	{
		auto interface = Data->GetAddressOf(driveCount);

		if ((interface) && rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) == 0)
		{
			continue;
		}
		else if ((interface) &&
				 rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) != 0)
		{
			return *this;
		}
	}

	return (AHCIDeviceInterface&)IDeviceObject<MountpointInterface*>::operator<<(
		Data);
}

/// @brief Input operator.
/// @param Data the disk mountpoint.
/// @return the class itself after operation.
AHCIDeviceInterface& AHCIDeviceInterface::operator>>(MountpointInterface* Data)
{
	if (!Data)
		return *this;

	for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount)
	{
		auto interface = Data->GetAddressOf(driveCount);

		// really check if it's ATA.
		if ((interface) && rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) == 0)
		{
			continue;
		}
		else if ((interface) &&
				 rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) != 0)
		{
			return *this;
		}
	}

	return (AHCIDeviceInterface&)IDeviceObject<MountpointInterface*>::operator>>(
		Data);
}

const UInt16& AHCIDeviceInterface::GetPortsImplemented()
{
	return this->fPortsImplemented;
}

Void AHCIDeviceInterface::SetPortsImplemented(const UInt16& pi)
{
	MUST_PASS(pi > 0);
	this->fPortsImplemented = pi;
}

const UInt32& AHCIDeviceInterface::GetIndex()
{
	return this->fDriveIndex;
}

Void AHCIDeviceInterface::SetIndex(const UInt32& drv)
{
	MUST_PASS(MountpointInterface::kDriveIndexInvalid != drv);
}