/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef __INC_PLIST_HPP__
#define __INC_PLIST_HPP__

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>
#include <NewKit/String.hpp>

namespace Kernel
{
	/// @brief handle to anything (number, ptr, string...)
	using PropertyId = UIntPtr;

	/// @brief Kernel property class.
	/// @example \Properties\SmpCores or \Properties\KernelVersion
	class Property
	{
	public:
		explicit Property(const StringView& sw);
		virtual ~Property();

	public:
		Property& operator=(const Property&) = default;
		Property(const Property&)			 = default;

		bool			  StringEquals(StringView& name);
		const PropertyId& GetPropertyById();

	private:
		Ref<StringView> fName;
		PropertyId		fAction;
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace Kernel

#endif // !__INC_PLIST_HPP__
