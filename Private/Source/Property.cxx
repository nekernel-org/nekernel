/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <CFKit/Property.hpp>

namespace hCore {
bool Property::StringEquals(StringView& name) {
  return m_sName && this->m_sName == name;
}

const PropertyId& Property::GetPropertyById() { return m_Action; }
}  // namespace hCore
