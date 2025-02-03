/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __INC_NETWORK_DEVICE_H__
#define __INC_NETWORK_DEVICE_H__

#include <KernelKit/DeviceMgr.h>
#include <NetworkKit/IP.h>

/// @note Can either work with: Ethernet, GPRS, WiFi

namespace Kernel
{
	struct NetworkDeviceCommand;
	class NetworkDevice;

	/**
	 * \brief Network device interface, establishes a connection to the NIC.
	 */
	class NetworkDevice final : public IDeviceObject<NetworkDeviceCommand>
	{
	public:
		NetworkDevice(void (*out)(NetworkDeviceCommand),
					  void (*in)(NetworkDeviceCommand),
					  void (*onCleanup)(void) = nullptr);

		~NetworkDevice() override;

	public:
		NetworkDevice& operator=(const NetworkDevice&) = default;
		NetworkDevice(const NetworkDevice&)			   = default;

	public:
		const Char* Name() const override;
		Boolean		Name(const Char* newStr);

	private:
		static constexpr auto cNetworkNameLen = 512;

		Void (*fCleanup)(void);
		Char fNetworkName[cNetworkNameLen];
	};

	struct NetworkDeviceCommand final
	{
		UInt32	CommandName;
		UInt32	CommandType;
		UInt32	CommandFlags;
		VoidPtr CommandBuffer;
		SizeT	CommandSizeBuffer;
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

	/// @brief LTE device.
	using LTENetworkDevice = NetworkDevice;
} // namespace Kernel

#include <NetworkKit/NetworkDevice.inl>

#endif // !__INC_NETWORK_DEVICE_H__
