/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/KString.h>
#include <NewKit/Utils.h>

/// @file KString.cc
/// @brief Kernel String manipulation file.

namespace Kernel {
Char* KString::Data() {
  return this->fData;
}

const Char* KString::CData() const {
  return const_cast<const Char*>(this->fData);
}

Size KString::Length() const {
  return this->fDataSz;
}

bool KString::operator==(const KString& rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < this->Length(); ++index) {
    if (rhs.fData[index] != this->fData[index]) return false;
  }

  return true;
}

bool KString::operator==(const Char* rhs) const {
  if (rt_string_len(rhs) != this->Length()) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] != this->fData[index]) return false;
  }

  return true;
}

bool KString::operator!=(const KString& rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < rhs.Length(); ++index) {
    if (rhs.fData[index] == this->fData[index]) return false;
  }

  return true;
}

bool KString::operator!=(const Char* rhs) const {
  if (rt_string_len(rhs) != this->Length()) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] == this->fData[index]) return false;
  }

  return true;
}

ErrorOr<KString> KStringBuilder::Construct(const Char* data) {
  if (!data || *data == 0) return {};

  KString* view = new KString(rt_string_len(data));
  (*view) += data;

  return ErrorOr<KString>(*view);
}

const Char* KStringBuilder::FromBool(const Char* fmt, bool i) {
  if (!fmt) return ("?");

  const Char* boolean_expr = i ? "YES" : "NO";
  Char*       ret          = (Char*) RTL_ALLOCA(rt_string_len(boolean_expr) + rt_string_len(fmt));

  if (!ret) return ("?");

  const auto fmt_len = rt_string_len(fmt);
  const auto res_len = rt_string_len(boolean_expr);

  for (Size idx = 0; idx < fmt_len; ++idx) {
    if (fmt[idx] == '%') {
      SizeT result_cnt = idx;

      for (auto y_idx = idx; y_idx < res_len; ++y_idx) {
        ret[result_cnt] = boolean_expr[y_idx];
        ++result_cnt;
      }

      break;
    }

    ret[idx] = fmt[idx];
  }

  return ret;
}

bool KStringBuilder::Equals(const Char* lhs, const Char* rhs) {
  if (rt_string_len(rhs) != rt_string_len(lhs)) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

/// @note This is unsafe!!!
bool KStringBuilder::Equals(const Utf8Char* lhs, const Utf8Char* rhs) {
  for (Size index = 0; index < urt_string_len(rhs); ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

bool KStringBuilder::Equals(const WideChar* lhs, const WideChar* rhs) {
  for (Size index = 0; rhs[index] != 0; ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

const Char* KStringBuilder::Format(const Char* fmt, const Char* fmt2) {
  if (!fmt || !fmt2) return ("?");

  Char* ret = (Char*) RTL_ALLOCA(sizeof(char) * (rt_string_len(fmt2) + rt_string_len(fmt)));

  if (!ret) return ("?");

  const auto len = rt_string_len(fmt);

  for (Size idx = 0; idx < len; ++idx) {
    if (fmt[idx] == '%' && idx < rt_string_len(fmt) && fmt[idx] == 's') {
      Size result_cnt = idx;

      for (Size y_idx = 0; y_idx < rt_string_len(fmt2); ++y_idx) {
        ret[result_cnt] = fmt2[y_idx];
        ++result_cnt;
      }
    }

    ret[idx] = fmt[idx];
  }

  return ret;
}

STATIC void rt_string_append(Char* lhs, const Char* rhs, Int32 cur) {
  SizeT sz_rhs = rt_string_len(rhs);
  SizeT rhs_i  = 0;

  for (; rhs_i < sz_rhs; ++rhs_i) {
    lhs[rhs_i + cur] = rhs[rhs_i];
  }
}

KString& KString::operator+=(const Char* rhs) {
  rt_string_append(this->fData, rhs, this->fCur);
  this->fCur += rt_string_len(rhs);

  return *this;
}

KString& KString::operator+=(const KString& rhs) {
  if (rt_string_len(rhs.fData) > this->Length()) return *this;

  rt_string_append(this->fData, const_cast<Char*>(rhs.fData), this->fCur);
  this->fCur += rt_string_len(const_cast<Char*>(rhs.fData));

  return *this;
}
}  // namespace Kernel
