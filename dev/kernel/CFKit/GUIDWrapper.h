/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>
#include <NeKit/Ref.h>
#include <NeKit/Stream.h>

/* GUID for C++ classes. */

#define kXRNNil "@{........-....-M...-N...-............}"

/// @brief eXtended Resource Namespace
namespace CF::XRN {
using namespace Kernel;

union GUIDSequence {
  alignas(8) UShort fU8[16];
  alignas(8) UShort fU16[8];
  alignas(8) UInt fU32[4];
  alignas(8) ULong fU64[2];

  struct GUID {
    alignas(8) UInt fMs1;
    UShort fMs2;
    UShort fMs3;
    UChar  fMs4[8];
  } fUuid;
};

class GUID final {
 public:
  explicit GUID() = default;
  ~GUID()         = default;

 public:
  GUID& operator=(const GUID&) = default;
  GUID(const GUID&)            = default;

 public:
  GUIDSequence& operator->() noexcept { return fUUID; }
  GUIDSequence& Leak() noexcept { return fUUID; }

 private:
  GUIDSequence fUUID;
};
}  // namespace CF::XRN
