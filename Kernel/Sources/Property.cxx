/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <CFKit/Property.hpp>

namespace Kernel
{
	Property::Property(const StringView& sw)
		: fName(sw)
	{
		kcout << "newoskrnl: Property created: " << sw.CData();
		endl;
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
} // namespace Kernel
