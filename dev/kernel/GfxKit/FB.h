/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>

namespace Kernel {
class FBDeviceInterface;
struct FBDevicePacket;

/// @brief Framebuffer device interface packet.
/// @details This structure is used to send and receive data from the framebuffer device.
/// @note The structure is packed to ensure that the data is aligned correctly for the device.
struct PACKED FBDevicePacket final {
  UInt32 fX;
  UInt32 fY;
  UInt32 fWidth;
  UInt32 fHeight;
  UInt32 fColor;
  UInt32 fFlags;
};

/// @brief Framebuffer device interface.
/// @details This class is used to send and receive data from the framebuffer device.
/// @note The class is derived from the IDeviceObject class.
class FBDeviceInterface NE_DEVICE<FBDevicePacket*> {
 public:
  explicit FBDeviceInterface(void (*out)(IDeviceObject* self, FBDevicePacket* out),
                             void (*in)(IDeviceObject* self, FBDevicePacket* in));

  virtual ~FBDeviceInterface() override;

 public:
  FBDeviceInterface& operator=(const FBDeviceInterface&) = default;
  FBDeviceInterface(const FBDeviceInterface&)            = default;

  const Char* Name() const override;

 public:
  FBDeviceInterface& operator<<(FBDevicePacket* Data) override;
  FBDeviceInterface& operator>>(FBDevicePacket* Data) override;
};
}  // namespace Kernel
