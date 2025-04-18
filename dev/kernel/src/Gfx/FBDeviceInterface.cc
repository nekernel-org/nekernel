/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <GfxKit/Gfx.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
FBDeviceInterface::FBDeviceInterface(void (*out)(IDeviceObject* self, FBDevicePacket* outpacket),
									 void (*in)(IDeviceObject* self, FBDevicePacket* inpacket))
	: IDeviceObject(out, in)
{
}

/// @brief Class desctructor
FBDeviceInterface::~FBDeviceInterface() = default;

/// @brief Output operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
FBDeviceInterface& FBDeviceInterface::operator<<(FBDevicePacket* pckt)
{
	if (!pckt)
		return *this;

	FBDrawInRegion(pckt->fColor, pckt->fHeight, pckt->fWidth, pckt->fY, pckt->fX);

	return *this;
}

/// @brief Input operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
FBDeviceInterface& FBDeviceInterface::operator>>(FBDevicePacket* pckt)
{
	if (!pckt)
		return *this;

	pckt->fColor = *(((Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +
										4 * kHandoverHeader->f_GOP.f_PixelPerLine *
											pckt->fX +
										4 * pckt->fY)));

	return *this;
}

/// @brief Returns the name of the device interface.
/// @return it's name as a string.
const Char* FBDeviceInterface::Name() const
{
	return "/dev/fb{}";
}