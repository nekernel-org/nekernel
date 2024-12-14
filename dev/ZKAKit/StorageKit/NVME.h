/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>

namespace Kernel
{
	class NVMEDeviceInterface final ZKA_DEVICE<MountpointInterface*>
	{
	public:
		explicit NVMEDeviceInterface(Void (*out)(MountpointInterface* out_packet),
									 Void (*in)(MountpointInterface* in_packet),
									 Void (*cleanup)(Void));

		~NVMEDeviceInterface() override;

	public:
		ZKA_COPY_DEFAULT(NVMEDeviceInterface);

		const Char* Name() const override;

	public:
		OwnPtr<MountpointInterface*> operator()(UInt32 dma_low, UInt32 dma_high, SizeT dma_sz);

	private:
		Void (*fCleanup)(Void) = {nullptr};
	};
} // namespace Kernel
