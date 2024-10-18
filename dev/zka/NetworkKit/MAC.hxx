/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/String.hxx>

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
