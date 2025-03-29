/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

	File: dev.h
	Purpose: DDK device support.

------------------------------------------- */

#pragma once

#include <DDKKit/ddk.h>

struct _DDK_DEVICE;

#define DDK_DEVICE_NAME_LEN (255)

/// @brief Kernel Device driver.
typedef struct _DDK_DEVICE DDK_FINAL
{
	char d_name[DDK_DEVICE_NAME_LEN];	 // the device name. Could be /./DEVICE_NAME/
	void* (*d_read)(void* arg, int len); // read from device.
	void (*d_write)(void* arg, int len);
	void (*d_wait)(void);							 // write to device.
	struct _DDK_DEVICE* (*d_open)(const char* path); // open device.
	void (*d_close)(struct _DDK_DEVICE* dev);		 // close device.
} DDK_DEVICE, *DDK_DEVICE_PTR;

/// @brief Open a new device from path.
/// @param path the device's path.
DDK_EXTERN DDK_DEVICE_PTR open(const char* path);

/// @brief Close any device.
/// @param device valid device.
DDK_EXTERN BOOL close(DDK_DEVICE_PTR device);
