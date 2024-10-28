/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <CFKit/Property.h>

namespace CFKit
{
	Property::~Property() = default;

	Bool Property::StringEquals(StringView& name)
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
} // namespace CFKit
