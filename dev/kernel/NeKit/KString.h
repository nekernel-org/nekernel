/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>
#include <NeKit/ErrorOr.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/Utils.h>

#define kMinimumStringSize (8196U)

namespace Kernel {
/// @brief Kernel string class, not dynamic.
template <SizeT MinSz = kMinimumStringSize>
class BasicKString final {
 public:
  explicit BasicKString() {
    fDataSz = MinSz;

    fData = new Char[fDataSz];
    MUST_PASS(fData);

    rt_set_memory(fData, 0, fDataSz);
  }

  explicit BasicKString(SizeT Sz) : fDataSz(Sz) {
    MUST_PASS(Sz > 1);

    fData = new Char[Sz];
    MUST_PASS(fData);

    rt_set_memory(fData, 0, Sz);
  }

  ~BasicKString() {
    if (fData) {
      delete[] fData;
      fData = nullptr;
    }
  }

  NE_COPY_DEFAULT(BasicKString)

  Char*       Data();
  const Char* CData() const;
  Size        Length() const;

  bool operator==(const Char* rhs) const;
  bool operator!=(const Char* rhs) const;

  bool operator==(const BasicKString<>& rhs) const;
  bool operator!=(const BasicKString<>& rhs) const;

  BasicKString<>& operator+=(const Char* rhs);
  BasicKString<>& operator+=(const BasicKString<>& rhs);

  operator const char*() { return fData; }

  operator bool() { return fData; }

  bool operator!() { return fData; }

 private:
  Char* fData{nullptr};
  Size  fDataSz{0};
  Size  fCur{0};

  friend class KStringBuilder;
};

using KString   = BasicKString<>;
using KStringOr = ErrorOr<KString>;

class KStringBuilder final {
 public:
  static ErrorOr<KString> Construct(const Char* data);
  static const Char*      FromBool(const Char* fmt, bool n);
  static const Char*      Format(const Char* fmt, const Char* from);
  static bool             Equals(const Char* lhs, const Char* rhs);
  static bool             Equals(const Utf8Char* lhs, const Utf8Char* rhs);
};
}  // namespace Kernel

#include <NeKit/KString.inl>
