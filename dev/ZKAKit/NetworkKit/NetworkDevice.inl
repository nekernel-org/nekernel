/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/***
	Dtor and ctors.
*/

namespace Kernel
{
	NetworkDevice::NetworkDevice(void (*out)(NetworkDeviceCommand),
								 void (*in)(NetworkDeviceCommand),
								 void (*on_cleanup)(void))
		: IDeviceObject<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup)
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
