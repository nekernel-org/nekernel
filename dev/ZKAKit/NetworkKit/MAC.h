/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>

namespace Kernel
{
	class MacAddressGetter;

	/// \brief This retrieves the MAC address of the device.
	/// \note Listens for the current NIC.
	class MacAddressGetter final
	{
	public:
		explicit MacAddressGetter() = default;

	public:
		KString&			 AsString();
		Array<WideChar, 12>& AsBytes();
	};

} // namespace Kernel
