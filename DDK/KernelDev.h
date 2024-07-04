/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Devices.

------------------------------------------- */

#pragma once

#include <DDK/KernelStd.h>

struct _kernelDevice;

/// @brief Kernel Device driver.
typedef struct _kernelDevice
{
	char name[255];					   // the device name. Could be /./DEVICE_NAME/
	void* (*read)(void* arg, int len); // read from device.
	void (*write)(void* arg, int len);
	void (*wait)(void);								 // write to device.
	struct _kernelDevice* (*open)(const char* path); // open device.
	void (*close)(struct _kernelDevice* dev);		 // close device.
} kernelDevice, *kernelDeviceRef;

/// @brief Open a new device from path.
/// @param devicePath the device's path.
DK_EXTERN kernelDeviceRef kernelOpenDevice(const char* devicePath);

/// @brief Close any device.
/// @param device valid device.
DK_EXTERN void kernelCloseDevice(kernelDeviceRef device);
