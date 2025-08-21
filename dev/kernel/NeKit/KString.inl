/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NeKit/Utils.h>

/// @file BasicKString<>.cc
/// @brief Kernel String manipulation file.

namespace Kernel {
inline void rt_string_append(Char* lhs, const Char* rhs, Int32 cur) {
  SizeT sz_rhs = rt_string_len(rhs);
  SizeT rhs_i  = 0;

  for (; rhs_i < sz_rhs; ++rhs_i) {
    lhs[rhs_i + cur] = rhs[rhs_i];
  }
}

template <>
inline Char* BasicKString<>::Data() {
  return this->fData;
}

template <>
inline const Char* BasicKString<>::CData() const {
  return const_cast<const Char*>(this->fData);
}

template <>
inline SizeT BasicKString<>::Length() const {
  return this->fDataSz;
}

template <>
inline bool BasicKString<>::operator==(const BasicKString<>& rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < this->Length(); ++index) {
    if (rhs.fData[index] != this->fData[index]) return false;
  }

  return true;
}

template <>
inline bool BasicKString<>::operator==(const Char* rhs) const {
  if (rt_string_len(rhs) != this->Length()) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] != this->fData[index]) return false;
  }

  return true;
}

template <>
inline bool BasicKString<>::operator!=(const BasicKString<>& rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < rhs.Length(); ++index) {
    if (rhs.fData[index] == this->fData[index]) return false;
  }

  return true;
}

template <>
inline bool BasicKString<>::operator!=(const Char* rhs) const {
  if (rt_string_len(rhs) != this->Length()) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] == this->fData[index]) return false;
  }

  return true;
}

template <>
inline BasicKString<>& BasicKString<>::operator+=(const BasicKString<>& rhs) {
  if (rt_string_len(rhs.fData) > this->Length()) return *this;

  rt_string_append(this->fData, const_cast<Char*>(rhs.fData), this->fCur);
  this->fCur += rt_string_len(const_cast<Char*>(rhs.fData));

  return *this;
}

template <>
inline BasicKString<>& BasicKString<>::operator+=(const Char* rhs) {
  rt_string_append(this->fData, const_cast<Char*>(rhs), this->fCur);
  this->fCur += rt_string_len(const_cast<Char*>(rhs));

  return *this;
}

inline ErrorOr<BasicKString<>> KStringBuilder::Construct(const Char* data) {
  if (!data || *data == 0) return ErrorOr<BasicKString<>>(new BasicKString<>(0));

  BasicKString<>* view = new BasicKString<>(rt_string_len(data));
  (*view) += data;

  return ErrorOr<BasicKString<>>(*view);
}

inline const Char* KStringBuilder::FromBool(const Char* fmt, bool i) {
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

inline bool KStringBuilder::Equals(const Char* lhs, const Char* rhs) {
  if (rt_string_len(rhs) != rt_string_len(lhs)) return false;

  for (Size index = 0; index < rt_string_len(rhs); ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

inline bool KStringBuilder::Equals(const Utf8Char* lhs, const Utf8Char* rhs) {
  if (urt_string_len(rhs) != urt_string_len(lhs)) return false;

  for (Size index = 0; rhs[index] != 0; ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

inline const Char* KStringBuilder::Format(const Char* fmt, const Char* fmt2) {
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
}  // namespace Kernel
