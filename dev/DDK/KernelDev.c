/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <DDK/KernelDev.h>
#include <DDK/KernelString.h>

/// @brief Open a new binary device from path.
DK_EXTERN KERNEL_DEVICE_PTR KernelOpenDevice(const char* devicePath)
{
	if (!devicePath)
		return nil;

	return KernelCall("OpenDevice", 1, (void*)devicePath, KernelStringLength(devicePath));
}

/// @brief Close any device.
/// @param device valid device.
DK_EXTERN void KernelCloseDevice(KERNEL_DEVICE_PTR device)
{
	if (!device)
		return;

	KernelCall("CloseDevice", 1, device, sizeof(KERNEL_DEVICE));
}
