/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

/***
	Dtor and ctors.
*/

namespace NewOS
{
	NetworkDevice::NetworkDevice(void (*out)(NetworkDeviceCommand),
								 void (*in)(NetworkDeviceCommand),
								 void (*on_cleanup)(void))
		: DeviceInterface<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup)
	{
		kcout << "New OS: NetworkDevice initialized.\r";

		MUST_PASS(out && in && on_cleanup);
	}

	NetworkDevice::~NetworkDevice()
	{
		MUST_PASS(fCleanup);

		kcout << "New OS: NetworkDevice cleanup.\r";
		if (fCleanup)
			fCleanup();
	}
} // namespace NewOS
