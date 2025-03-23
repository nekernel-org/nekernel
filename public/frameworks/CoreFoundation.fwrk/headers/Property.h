/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef _PROPS_H
#define _PROPS_H

#include <SCIKit/SCI.h>
#include <CoreFoundation.fwrk/headers/Ref.h>

#define kMaxPropLen (256U)

namespace LibCF
{
	class CFString;
	class CFProperty;
	class CFGUID;

	template <typename Cls, SizeT N>
	class CFArray;

	/// @brief handle to anything (number, ptr, string...)
	using CFPropertyId = UIntPtr;

	/// @brief User property class.
	/// @example /prop/foo or /prop/bar
	class CFProperty final CF_OBJECT
	{
	public:
		CFProperty();
		virtual ~CFProperty();

	public:
		CFProperty& operator=(const CFProperty&) = default;
		CFProperty(const CFProperty&)			 = default;

		Bool		  StringEquals(CFString& name);
		CFPropertyId& GetValue();
		CFString&	  GetKey();

	private:
		CFString*	 fName{nullptr};
		CFPropertyId fValue{0UL};
		Ref<CFGUID>	 fGUID{};
	};

	template <SizeT N>
	using CFPropertyArray = CFArray<CFProperty, N>;
} // namespace LibCF

#endif // !CFKIT_PROPS_H
