/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <CFKit/Property.hpp>

namespace NewOS {
bool Property::StringEquals(StringView& name) {
  return fName && this->fName == name;
}

const PropertyId& Property::GetPropertyById() { return fAction; }
}  // namespace NewOS
