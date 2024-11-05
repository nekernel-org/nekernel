/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

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
		Property() = default;
		virtual ~Property();

	public:
		Property& operator=(const Property&) = default;
		Property(const Property&)			 = default;

		bool		StringEquals(KString& name);
		PropertyId& GetValue();
		KString&	GetKey();

	private:
		KString	   fName{kMaxPropLen};
		PropertyId fAction{No};
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace CFKit

namespace Kernel
{
	using namespace CFKit;
}

#endif // !CFKIT_PROPS_H
