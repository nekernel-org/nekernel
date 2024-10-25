/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef __INC_PROPS_H__
#define __INC_PROPS_H__

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>
#include <NewKit/String.h>

#define cMaxPropLen 4096

namespace CFKit
{
	using namespace Kernel;

	/// @brief handle to anything (number, ptr, string...)
	using PropertyId = UIntPtr;

	/// @brief Kernel property class.
	/// @example \Properties\SmpCores or \Properties\KernelVersion
	class Property
	{
	public:
		Property() = default;
		virtual ~Property();

	public:
		Property& operator=(const Property&) = default;
		Property(const Property&)			 = default;

		bool		StringEquals(StringView& name);
		PropertyId& GetValue();
		StringView& GetKey();

	private:
		StringView fName{cMaxPropLen};
		PropertyId fAction{No};
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace CFKit

namespace Kernel
{
	using namespace CFKit;
}

#endif // !__INC_PROPS_H__
