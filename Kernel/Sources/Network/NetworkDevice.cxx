/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <NetworkKit/NetworkDevice.hpp>
#include <NewKit/Utils.hpp>

namespace Kernel
{
	/// \brief Getter for fNetworkName.
	const char* NetworkDevice::Name() const
	{
		return this->fNetworkName;
	}

	/// \brief Setter for fNetworkName.
	Boolean NetworkDevice::Name(const char* strView)
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
