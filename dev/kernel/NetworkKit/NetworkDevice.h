/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __INC_NETWORK_DEVICE_H__
#define __INC_NETWORK_DEVICE_H__

#include <KernelKit/DeviceMgr.h>
#include <NetworkKit/IP.h>

/// @note Can either work with: Ethernet, GPRS, WiFi

namespace Kernel {
struct NetworkDeviceCommand;
class NetworkDevice;

/**
 * \brief Network device interface, establishes a connection to the NIC.
 */
class NetworkDevice final NE_DEVICE<NetworkDeviceCommand> {
 public:
  NetworkDevice(void (*out)(DeviceInterface<NetworkDeviceCommand>*, NetworkDeviceCommand),
                void (*in)(DeviceInterface<NetworkDeviceCommand>*, NetworkDeviceCommand),
                void (*cleanup)(void) = nullptr);

  ~NetworkDevice() override;

 public:
  NetworkDevice& operator=(const NetworkDevice&) = default;
  NetworkDevice(const NetworkDevice&)            = default;

 public:
  const Char* Name() const override;
  Boolean     Name(const Char* newStr);

 private:
  Void (*fCleanup)(void);
};

struct NetworkDeviceCommand final {
  UInt32  CommandName;
  UInt32  CommandType;
  UInt32  CommandFlags;
  VoidPtr CommandBuffer;
  SizeT   CommandSizeBuffer;
};

/// @brief TCP device.
using TCPNetworkDevice = NetworkDevice;

/// @brief UDP device.
using UDPNetworkDevice = NetworkDevice;

/// @brief PPP device.
using PPPNetworkDevice = NetworkDevice;

/// @brief IPC device.
using IPCNetworkDevice = NetworkDevice;

/// @brief GRPS device.
using GPRSNetworkDevice = NetworkDevice;

/// @brief GSM device.
using GSMNetworkDevice = NetworkDevice;

/// @brief Bluetooth device.
using BTNetworkDevice = NetworkDevice;

/// @brief Printer device.
using PrinterNetworkDevice = NetworkDevice;

/// @brief Debug device.
using DBGNetworkDevice = NetworkDevice;

/// @brief LTE device.
using LTENetworkDevice = NetworkDevice;
}  // namespace Kernel

#include <NetworkKit/NetworkDevice.inl>

#endif  // !__INC_NETWORK_DEVICE_H__
