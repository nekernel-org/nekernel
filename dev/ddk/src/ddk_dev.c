/* -------------------------------------------

	Copyright Amlal El Mahrouss.

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <DDKKit/dev.h>
#include <DDKKit/str.h>

/// @brief Open a new binary device from path.
DDK_EXTERN DDK_DEVICE_PTR open(const char* devicePath)
{
	if (!devicePath)
		return nil;

	return ke_call("sk_open_dev", 1, (void*)devicePath, kstrlen(devicePath));
}

/// @brief Close any device.
/// @param device valid device.
DDK_EXTERN BOOL close(DDK_DEVICE_PTR device)
{
	if (!device)
		return NO;

	ke_call("sk_close_dev", 1, device, sizeof(DDK_DEVICE));
	return YES;
}
