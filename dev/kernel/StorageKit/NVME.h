/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>

namespace Kernel
{
	class NVMEDeviceInterface final NE_DEVICE<MountpointInterface*>
	{
	public:
		explicit NVMEDeviceInterface(Void (*out)(IDeviceObject*, MountpointInterface* out_packet),
									 Void (*in)(IDeviceObject*, MountpointInterface* in_packet),
									 Void (*cleanup)(Void));

		~NVMEDeviceInterface() override;

	public:
		NE_COPY_DEFAULT(NVMEDeviceInterface)

		const Char* Name() const override;

	public:
		OwnPtr<MountpointInterface*> operator()(UInt32 dma_low, UInt32 dma_high, SizeT dma_sz);

	private:
		Void (*fCleanup)(Void) = {nullptr};
	};
} // namespace Kernel
