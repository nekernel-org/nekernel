/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NetworkKit/MAC.h>

namespace Kernel
{
	Array<UInt8, kMACAddrLen>& MacAddressGetter::AsBytes()
	{
		return this->fMacAddress;
	}
} // namespace Kernel
