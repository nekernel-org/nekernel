/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/Utils.h>

namespace Kernel
{
	/// @brief ATA device interface type.
	class ATADeviceInterface : public IDeviceObject<MountpointInterface*>
	{
	public:
		explicit ATADeviceInterface(void (*Out)(IDeviceObject*, MountpointInterface* outpacket),
									void (*In)(IDeviceObject*, MountpointInterface* inpacket),
									void (*Cleanup)(void));

		virtual ~ATADeviceInterface();

	public:
		ATADeviceInterface& operator<<(MountpointInterface* Data) override;
		ATADeviceInterface& operator>>(MountpointInterface* Data) override;

	public:
		ATADeviceInterface& operator=(const ATADeviceInterface&) = default;
		ATADeviceInterface(const ATADeviceInterface&)			 = default;

		const Char* Name() const override;

	private:
		void (*fCleanup)(void) = {nullptr};
	};
} // namespace Kernel
