/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_PLIST_HPP__
#define _INC_PLIST_HPP__

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>
#include <NewKit/String.hpp>

namespace HCore {
using PropertyId = Int;

class Property {
 public:
  explicit Property(const StringView &sw) : m_sName(sw) {}

  virtual ~Property() = default;

 public:
  Property &operator=(const Property &) = default;
  Property(const Property &) = default;

  bool StringEquals(StringView &name);
  const PropertyId &GetPropertyById();

 private:
  Ref<StringView> m_sName;
  PropertyId m_Action;
};

template <SSizeT N>
using PropertyArray = Array<Property, N>;
}  // namespace HCore

#endif  // !_INC_PLIST_HPP__
