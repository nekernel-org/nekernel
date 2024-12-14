/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/OwnPtr.h>

namespace Kernel
{
	class AHCIDeviceInterface ZKA_DEVICE<MountpointInterface*>
	{
	public:
		explicit AHCIDeviceInterface(void (*Out)(MountpointInterface* outpacket),
									 void (*In)(MountpointInterface* inpacket),
									 void (*Cleanup)(void));

		virtual ~AHCIDeviceInterface();

	public:
		AHCIDeviceInterface& operator=(const AHCIDeviceInterface&) = default;
		AHCIDeviceInterface(const AHCIDeviceInterface&)			   = default;

		const Char* Name() const override;

	private:
		void (*fCleanup)(void) = {nullptr};
	};
} // namespace Kernel
