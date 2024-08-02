/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hxx>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/OwnPtr.hxx>
#include <NewKit/Utils.hxx>

namespace Kernel
{
	/// @brief ATA device interface type.
	class ATADeviceInterface : public DeviceInterface<MountpointInterface*>
	{
	public:
		explicit ATADeviceInterface(void (*Out)(MountpointInterface* outpacket),
									void (*In)(MountpointInterface* inpacket),
									void (*Cleanup)(void));

		virtual ~ATADeviceInterface();

	public:
		ATADeviceInterface& operator<<(MountpointInterface* Data) override;
		ATADeviceInterface& operator>>(MountpointInterface* Data) override;

	public:
		ATADeviceInterface& operator=(const ATADeviceInterface&) = default;
		ATADeviceInterface(const ATADeviceInterface&)			 = default;

		const char* Name() const override;

	private:
		void (*fCleanup)(void) = { nullptr };
	};
} // namespace Kernel
