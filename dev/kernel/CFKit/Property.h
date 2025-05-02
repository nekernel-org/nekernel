/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef CFKIT_PROPS_H
#define CFKIT_PROPS_H

#include <CFKit/GUIDWrapper.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>
#include <NewKit/KString.h>

#define kMaxPropLen (256U)

namespace CF {
using namespace Kernel;

/// @brief handle to anything (number, ptr, string...)
using PropertyId = UIntPtr;

/// @brief Kernel property class.
/// @example /prop/smp_max or /prop/kern_ver
class Property {
 public:
  Property();
  virtual ~Property();

 public:
  Property& operator=(const Property&) = default;
  Property(const Property&)            = default;

  BOOL        StringEquals(KString& name);
  PropertyId& GetValue();
  KString&    GetKey();

 private:
  KString        fName{kMaxPropLen};
  PropertyId     fValue{0UL};
  Ref<XRN::GUID> fGUID{};
};

template <SizeT N>
using PropertyArray = Array<Property, N>;
}  // namespace CF

namespace Kernel {
using namespace CF;
}

#endif  // !CFKIT_PROPS_H
