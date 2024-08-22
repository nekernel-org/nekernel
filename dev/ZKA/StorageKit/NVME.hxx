/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hxx>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/OwnPtr.hxx>

namespace Kernel
{
	class NVMEDeviceInterface final : public DeviceInterface<MountpointInterface*>
	{
	public:
		explicit NVMEDeviceInterface(void (*Out)(MountpointInterface* outpacket),
									 void (*In)(MountpointInterface* inpacket),
									 void (*Cleanup)(void));

		~NVMEDeviceInterface() override;

	public:
		NVMEDeviceInterface& operator=(const NVMEDeviceInterface&) = default;
		NVMEDeviceInterface(const NVMEDeviceInterface&)			   = default;

		const Char* Name() const override;

	public:
		OwnPtr<MountpointInterface*> operator()(UInt32 dmaLow, UInt32 dmaHigh, SizeT sz);

	private:
		void (*fCleanup)(void) = {nullptr};
	};
} // namespace Kernel
