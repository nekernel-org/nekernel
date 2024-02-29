/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/KernelCheck.hpp>

namespace HCore {
class StringView final {
 public:
  explicit StringView() = default;

  explicit StringView(Size Sz) : m_Sz(Sz) {
    MUST_PASS(Sz > 1);
    m_Data = new Char[Sz];
    MUST_PASS(m_Data);
  }

  ~StringView() {
    if (m_Data) delete[] m_Data;
  }

  StringView &operator=(const StringView &) = default;
  StringView(const StringView &) = default;

  Char *Data();
  const Char *CData();
  Size Length() const;

  bool operator==(const Char *rhs) const;
  bool operator!=(const Char *rhs) const;

  bool operator==(const StringView &rhs) const;
  bool operator!=(const StringView &rhs) const;

  StringView &operator+=(const Char *rhs);
  StringView &operator+=(const StringView &rhs);

  operator bool() { return m_Data; }

  bool operator!() { return m_Data; }

 private:
  Char *m_Data{nullptr};
  Size m_Sz{0};
  Size m_Cur{0};

  friend class StringBuilder;
};

struct StringBuilder final {
  static ErrorOr<StringView> Construct(const Char *data);
  static const char *FromInt(const char *fmt, int n);
  static const char *FromBool(const char *fmt, bool n);
  static const char *Format(const char *fmt, const char *from);
  static bool Equals(const char *lhs, const char *rhs);
};
}  // namespace HCore
