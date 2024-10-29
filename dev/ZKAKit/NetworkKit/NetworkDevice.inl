/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

/***
	Dtor and ctors.
*/

namespace Kernel
{
	NetworkDevice::NetworkDevice(void (*out)(NetworkDeviceCommand),
								 void (*in)(NetworkDeviceCommand),
								 void (*on_cleanup)(void))
		: DeviceInterface<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup)
	{
		kcout << "NetworkDevice initialized.\r";

		MUST_PASS(out && in && on_cleanup);
	}

	NetworkDevice::~NetworkDevice()
	{
		MUST_PASS(fCleanup);

		kcout << "NetworkDevice cleanup.\r";

		if (fCleanup)
			fCleanup();
	}
} // namespace Kernel
