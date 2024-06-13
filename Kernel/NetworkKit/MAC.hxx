/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

namespace NewOS
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

} // namespace NewOS
