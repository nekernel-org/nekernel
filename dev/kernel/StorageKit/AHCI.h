/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/OwnPtr.h>

namespace Kernel
{
	/// @brief AHCIDeviceInterface class
	/// @details This class is used to send and receive data from the AHCI device.
	/// @note The class is derived from the IDeviceObject class.
	class AHCIDeviceInterface NE_DEVICE<MountpointInterface*>
	{
	public:
		explicit AHCIDeviceInterface(void (*out)(IDeviceObject* self, MountpointInterface* out),
									 void (*in)(IDeviceObject* self, MountpointInterface* in));

		virtual ~AHCIDeviceInterface() override;

	public:
		AHCIDeviceInterface& operator=(const AHCIDeviceInterface&) = default;
		AHCIDeviceInterface(const AHCIDeviceInterface&)			   = default;

		const Char* Name() const override;

		const UInt16& GetPortsImplemented();

		Void SetPortsImplemented(const UInt16& pi);

		const UInt32& GetIndex();

		Void SetIndex(const UInt32& drv);

	public:
		AHCIDeviceInterface& operator<<(MountpointInterface* Data) override;
		AHCIDeviceInterface& operator>>(MountpointInterface* Data) override;

	private:
		UInt16 fPortsImplemented{0U};
		UInt32 fDriveIndex{0U};
	};

	UInt16						 sk_init_ahci_device(BOOL atapi);
	ErrorOr<AHCIDeviceInterface> sk_acquire_ahci_device(Int32 drv_index);
} // namespace Kernel
