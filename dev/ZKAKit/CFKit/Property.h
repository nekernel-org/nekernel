/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef CFKIT_PROPS_H
#define CFKIT_PROPS_H

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>
#include <NewKit/KString.h>

#define kMaxPropLen 255

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
		Property();
		virtual ~Property();

	public:
		Property& operator=(const Property&) = default;
		Property(const Property&)			 = default;

		bool		StringEquals(KString& name);
		PropertyId& GetValue();
		KString&	GetKey();

	private:
		KString	   fName{kMaxPropLen};
		PropertyId fValue{0UL};
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace CFKit

namespace Kernel
{
	using namespace CFKit;
}

#endif // !CFKIT_PROPS_H
