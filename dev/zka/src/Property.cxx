/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <CFKit/Property.hxx>

namespace Kernel
{
	Property::~Property() = default;

	bool Property::StringEquals(StringView& name)
	{
		return this->fName && this->fName == name;
	}

	StringView& Property::GetKey()
	{
		return this->fName;
	}

	PropertyId& Property::GetValue()
	{
		return fAction;
	}
} // namespace Kernel
