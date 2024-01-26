/*
 *	========================================================
 *
 *	hCore
 * 	Copyright XPX Corp, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_NETWORKDEVICE_HPP__
#define _INC_NETWORKDEVICE_HPP__

#include <KernelKit/Device.hpp>
#include <NetworkKit/IP.hpp>

namespace hCore
{
    struct NetworkDeviceCommand;

    class NetworkDevice final : public DeviceInterface<NetworkDeviceCommand>
    {
    public:
        NetworkDevice(void(*out)(NetworkDeviceCommand), void(*in)(NetworkDeviceCommand),
                               void(*on_cleanup)(void) = nullptr);

        ~NetworkDevice() override;

    public:
        NetworkDevice &operator=(const NetworkDevice &) = default;
        NetworkDevice(const NetworkDevice &) = default;

    public:
        const char* Name() const override { return ("NetworkDevice"); }

    private:
        void(*fCleanup)(void);

    };

    struct NetworkDeviceCommand
    {
        UInt32 Command;
        UInt32 VLan;
        UInt32 DmaLow;
        UInt32 DmaHigh;
    };

    using TCPNetworkDevice = NetworkDevice;
    using UDPNetworkDevice = NetworkDevice;
    using PPPNetworkDevice = NetworkDevice;
} // namespace hCore

#endif // !_INC_NETWORKDEVICE_HPP__
