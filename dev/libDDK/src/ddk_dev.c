/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK Text I/O.

------------------------------------------- */

#include <DriverKit/dev.h>
#include <DriverKit/str.h>

/// @brief Open a new binary device from path.
DDK_EXTERN DDK_DEVICE_PTR open(const char* devicePath) {
  if (nil == devicePath) return nil;

  return ke_call_dispatch("dk_open_dev", 1, (void*) devicePath, kstrlen(devicePath));
}

/// @brief Close any device.
/// @param device valid device.
DDK_EXTERN BOOL close(DDK_DEVICE_PTR device) {
  if (nil == device) return NO;

  ke_call_dispatch("dk_close_dev", 1, device, sizeof(DDK_DEVICE));
  return YES;
}
