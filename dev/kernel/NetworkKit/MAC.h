/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>

#define kMACAddrLen (32)

namespace Kernel
{
	class MacAddressGetter;

	/// \brief This retrieves the MAC address of the device.
	/// \note Listens for the current NIC.
	class MacAddressGetter final
	{
	public:
		MacAddressGetter()	= default;
		~MacAddressGetter() = default;

		NE_COPY_DEFAULT(MacAddressGetter)

	public:
		Array<UInt8, kMACAddrLen>& AsBytes();

	private:
		Array<UInt8, kMACAddrLen> fMacAddress;
	};

} // namespace Kernel
