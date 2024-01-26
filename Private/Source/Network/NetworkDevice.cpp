/*
 *	========================================================
 *
 *	NewKit
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NetworkKit/NetworkDevice.hpp>

// network devices implementation.
// PPPNetworkService, TCPNetworkDevice, UDPNetworkService

namespace hCore
{
    NetworkDevice::NetworkDevice(void (*out)(NetworkDeviceCommand), void (*in)(NetworkDeviceCommand), void(*on_cleanup)(void))
            : DeviceInterface<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup)
    {
#ifdef __DEBUG__
        kcout << "NetworkDevice init.\r\n";
#endif
    }

    NetworkDevice::~NetworkDevice()
    {
#ifdef __DEBUG__
        kcout << "NetworkDevice cleanup.\r\n";
#endif

        if (fCleanup)
            fCleanup();
    }
} // namespace NewKit
