/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _INC_NETWORKDEVICE_HPP__
#define _INC_NETWORKDEVICE_HPP__

#include <KernelKit/DeviceManager.hpp>
#include <NetworkKit/IP.hpp>

namespace HCore {
struct NetworkDeviceCommand;
class NetworkDevice;

/**
* \brief Network device interface, establishes a connection to the NIC.
*/
class NetworkDevice final : public DeviceInterface<NetworkDeviceCommand> {
 public:
  NetworkDevice(void (*out)(NetworkDeviceCommand),
                void (*in)(NetworkDeviceCommand),
                void (*onCleanup)(void) = nullptr);

  ~NetworkDevice() override;

 public:
  NetworkDevice &operator=(const NetworkDevice &) = default;
  NetworkDevice(const NetworkDevice &) = default;

 public:
  const char *Name() const override { return "NetworkDevice"; }

 private:
  void (*fCleanup)(void);
};

struct PACKED NetworkDeviceCommand final {
  UInt32 Command;
  UInt32 VLan;
  UInt32 DmaLow;
  UInt32 DmaHigh;
};

/// @brief TCP device.
using TCPNetworkDevice = NetworkDevice;

/// @brief UDP device.
using UDPNetworkDevice = NetworkDevice;

/// @brief PPP device.
using PPPNetworkDevice = NetworkDevice;

/// @brief HPC device.
using HPCNetworkDevice = NetworkDevice;
}  // namespace HCore

#include <NetworkKit/NetworkDevice.inl>

#endif  // !_INC_NETWORKDEVICE_HPP__
