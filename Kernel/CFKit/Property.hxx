/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __INC_PROPS_HPP__
#define __INC_PROPS_HPP__

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Function.hxx>
#include <NewKit/String.hxx>

#define cMaxPropLen 4096

namespace Kernel
{
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
		StringView      fName{cMaxPropLen};
		PropertyId		fAction{No};
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace Kernel

#endif // !__INC_PROPS_HPP__
