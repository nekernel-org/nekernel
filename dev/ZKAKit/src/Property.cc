/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

------------------------------------------- */

#include <CFKit/Property.h>

namespace CFKit
{
	Property::~Property() = default;

	Bool Property::StringEquals(KString& name)
	{
		return this->fName && this->fName == name;
	}

	KString& Property::GetKey()
	{
		return this->fName;
	}

	PropertyId& Property::GetValue()
	{
		return fAction;
	}
} // namespace CFKit
