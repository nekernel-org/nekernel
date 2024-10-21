/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/String.h>

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
		StringView&			 AsString();
		Array<WideChar, 12>& AsBytes();
	};

} // namespace Kernel
