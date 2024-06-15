/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <CFKit/Property.hpp>

namespace NewOS
{
	Property::Property(const StringView& sw)
	: fName(sw)
	{
		kcout << "Property: created: " << sw.CData() << endl;
	}

	Property::~Property() = default;

	bool Property::StringEquals(StringView& name)
	{
		return fName && this->fName == name;
	}

	const PropertyId& Property::GetPropertyById()
	{
		return fAction;
	}
} // namespace NewOS
