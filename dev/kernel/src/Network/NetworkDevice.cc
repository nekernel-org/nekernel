/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <NetworkKit/NetworkDevice.h>
#include <NewKit/Utils.h>

namespace Kernel
{
	/// \brief Getter for fNetworkName.
	const Char* NetworkDevice::Name() const
	{
		return this->fNetworkName;
	}

	/// \brief Setter for fNetworkName.
	Boolean NetworkDevice::Name(const Char* devnam)
	{
		if (devnam == nullptr)
			return NO;

		if (*devnam == 0)
			return NO;

		if (rt_string_len(devnam) > cNetworkNameLen)
			return NO;

		rt_copy_memory((VoidPtr)devnam,
					   (VoidPtr)this->fNetworkName, rt_string_len(devnam));

		return YES;
	}
} // namespace Kernel
