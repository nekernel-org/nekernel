/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NetworkKit/MAC.h>

namespace Kernel
{
	Array<WideChar, kMACAddrLen>& MacAddressGetter::AsBytes()
	{
		return this->fMacAddress;
	}
} // namespace Kernel
