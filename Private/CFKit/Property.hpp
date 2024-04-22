/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _INC_PLIST_HPP__
#define _INC_PLIST_HPP__

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>
#include <NewKit/String.hpp>

namespace NewOS {
using PropertyId = Int;

/// @brief Kernel property class.
class Property {
 public:
  explicit Property(const StringView &sw) : fName(sw) {}

  virtual ~Property() = default;

 public:
  Property &operator=(const Property &) = default;
  Property(const Property &) = default;

  bool StringEquals(StringView &name);
  const PropertyId &GetPropertyById();

 private:
  Ref<StringView> fName;
  PropertyId fAction;
};

template <SSizeT N>
using PropertyArray = Array<Property, N>;
}  // namespace NewOS

#endif  // !_INC_PLIST_HPP__
