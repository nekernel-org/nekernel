/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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
	Boolean NetworkDevice::Name(const Char* strView)
	{
		if (strView == nullptr)
			return false;

		if (*strView == 0)
			return false;

		if (rt_string_len(strView) > cNetworkNameLen)
			return false;

		rt_copy_memory((VoidPtr)strView,
					   (VoidPtr)this->fNetworkName, rt_string_len(strView));

		return true;
	}
} // namespace Kernel
