/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __NETWORK_DEVICE__
#define __NETWORK_DEVICE__

#include <KernelKit/DeviceManager.hpp>
#include <NetworkKit/IP.hpp>

namespace NewOS {
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
  const char *Name() const override;

 private:
  void (*fCleanup)(void);
};

struct PACKED NetworkDeviceCommand final {
  UInt32 CommandName;
  UInt32 CommandType;
  UInt32 CommandFlags;
  VoidPtr CommandBuffer;
  SizeT CommandSizeBuffer;
};

/// @brief TCP device.
using TCPNetworkDevice = NetworkDevice;

/// @brief UDP device.
using UDPNetworkDevice = NetworkDevice;

/// @brief PPP device.
using PPPNetworkDevice = NetworkDevice;

/// @brief HPC device.
using HPCNetworkDevice = NetworkDevice;
}  // namespace NewOS

#include <NetworkKit/NetworkDevice.inl>

#endif  // !__NETWORK_DEVICE__
