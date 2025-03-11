/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <NetworkKit/MAC.h>

namespace NeOS
{
	Array<WideChar, kMACAddrLen>& MacAddressGetter::AsBytes()
	{
        return this->fMacAddress;
	}
} // namespace NeOS
