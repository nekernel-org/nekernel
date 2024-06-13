/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Text I/O.

------------------------------------------- */

#include <DDK/KernelDev.h>

/// @brief Open a new binary device from path.
DK_EXTERN kernelDeviceRef kernelOpenBinaryDevice(const char* devicePath)
{
	if (!devicePath)
		return NIL;

	return kernelCall("OpenBinaryDevice", 1, devicePath);
}

/// @brief Open a new character device from path.
DK_EXTERN kernelDeviceRef kernelOpenCharDevice(const char* devicePath)
{
	if (!devicePath)
		return NIL;

	return kernelCall("OpenCharDevice", 1, devicePath);
}

/// @brief Close any device.
/// @param device valid device.
DK_EXTERN void kernelCloseDevice(kernelDeviceRef device)
{
	if (!device)
		return;

	kernelCall("CloseDevice", 1, device);
}
