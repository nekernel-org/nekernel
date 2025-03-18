/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/OwnPtr.h>

namespace NeOS
{
	class AHCIDeviceInterface NE_DEVICE<MountpointInterface*>
	{
	public:
		explicit AHCIDeviceInterface(void (*out)(IDeviceObject* self, MountpointInterface* out),
									 void (*in)(IDeviceObject* self, MountpointInterface* in),
									 void (*cleanup)(void));

		virtual ~AHCIDeviceInterface() override;

	public:
		AHCIDeviceInterface& operator=(const AHCIDeviceInterface&) = default;
		AHCIDeviceInterface(const AHCIDeviceInterface&)			   = default;

		const Char* Name() const override;

		const UInt16& GetPi()
		{
			return this->fPortsImplemented;
		}

		Void SetPi(const UInt16& pi)
		{
			MUST_PASS(pi > 0);
			this->fPortsImplemented = pi;
		}

		const UInt32& GetIndex()
		{
			return this->fDriveIndex;
		}

		Void SetIndex(const UInt32& drv)
		{
			this->fDriveIndex = drv;
		}

	private:
		Void (*fCleanup)(Void) = {nullptr};
		UInt16 fPortsImplemented{0U};
		UInt32 fDriveIndex{0U};
	};

	UInt16						 sk_init_ahci_device(BOOL atapi);
	ErrorOr<AHCIDeviceInterface> sk_acquire_ahci_device(Int32 drv_index);
} // namespace NeOS
