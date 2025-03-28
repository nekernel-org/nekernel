/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef CFKIT_PROPS_H
#define CFKIT_PROPS_H

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>
#include <NewKit/KString.h>
#include <CFKit/GUIDWrapper.h>

#define kMaxPropLen (256U)

namespace CFKit
{
	using namespace Kernel;

	/// @brief handle to anything (number, ptr, string...)
	using PropertyId = UIntPtr;

	/// @brief Kernel property class.
	/// @example /prop/smp_max or /prop/kern_ver
	class Property
	{
	public:
		Property();
		virtual ~Property();

	public:
		Property& operator=(const Property&) = default;
		Property(const Property&)			 = default;

		BOOL		StringEquals(KString& name);
		PropertyId& GetValue();
		KString&	GetKey();

	private:
		KString		   fName{kMaxPropLen};
		PropertyId	   fValue{0UL};
		Ref<XRN::GUID> fGUID{};
	};

	template <SizeT N>
	using PropertyArray = Array<Property, N>;
} // namespace CFKit

namespace Kernel
{
	using namespace CFKit;
}

#endif // !CFKIT_PROPS_H
