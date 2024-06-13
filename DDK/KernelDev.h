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
	char name[255];									 // the device name. Could be /./DEVICE_NAME/
	int32_t (*read)();								 // read from device.
	int32_t (*write)();								 // write to device.
	struct _kernelDevice* (*open)(const char* path); // open device.
	void (*close)(struct _kernelDevice* dev);		 // close device.
} kernelDevice, *kernelDeviceRef;

/// @brief Open a new binary device from path.
DK_EXTERN kernelDeviceRef kernelOpenBinaryDevice(const char* devicePath);

/// @brief Open a new character device from path.
DK_EXTERN kernelDeviceRef kernelOpenCharDevice(const char* devicePath);

/// @brief Close any device.
/// @param device valid device.
DK_EXTERN void kernelCloseDevice(kernelDeviceRef device);
