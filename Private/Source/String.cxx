/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>

namespace HCore {
Char *StringView::Data() { return m_Data; }

const Char *StringView::CData() { return m_Data; }

Size StringView::Length() const { return string_length(m_Data); }

bool StringView::operator==(const StringView &rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < this->Length(); ++index) {
    if (rhs.m_Data[index] != m_Data[index]) return false;
  }

  return true;
}

bool StringView::operator==(const Char *rhs) const {
  if (string_length(rhs) != this->Length()) return false;

  for (Size index = 0; index < string_length(rhs); ++index) {
    if (rhs[index] != m_Data[index]) return false;
  }

  return true;
}

bool StringView::operator!=(const StringView &rhs) const {
  if (rhs.Length() != this->Length()) return false;

  for (Size index = 0; index < rhs.Length(); ++index) {
    if (rhs.m_Data[index] == m_Data[index]) return false;
  }

  return true;
}

bool StringView::operator!=(const Char *rhs) const {
  if (string_length(rhs) != this->Length()) return false;

  for (Size index = 0; index < string_length(rhs); ++index) {
    if (rhs[index] == m_Data[index]) return false;
  }

  return true;
}

ErrorOr<StringView> StringBuilder::Construct(const Char *data) {
  if (!data || *data == 0) return {};

  StringView view(string_length(data));

  rt_copy_memory(reinterpret_cast<voidPtr>(const_cast<Char *>(data)),
                 reinterpret_cast<voidPtr>(view.Data()), view.Length());

  return ErrorOr<StringView>(view);
}

const char *StringBuilder::FromInt(const char *fmt, int i) {
  if (!fmt) return ("-1");

  char *ret = (char *)__alloca(sizeof(char) * 8 + string_length(fmt));

  if (!ret) return ("-1");

  Char result[8];

  if (!rt_to_string(result, sizeof(int), i)) {
    return ("-1");
  }

  const auto fmt_len = string_length(fmt);
  const auto res_len = string_length(result);

  for (Size idx = 0; idx < fmt_len; ++idx) {
    if (fmt[idx] == '%') {
      SizeT result_cnt = idx;

      for (auto y_idx = idx; y_idx < res_len; ++y_idx) {
        ret[result_cnt] = result[y_idx];
        ++result_cnt;
      }

      break;
    }

    ret[idx] = fmt[idx];
  }

  return ret; /* Copy that ret into a buffer, Alloca allocates to the stack */
}

const char *StringBuilder::FromBool(const char *fmt, bool i) {
  if (!fmt) return ("?");

  const char *boolean_expr = i ? "true" : "false";
  char *ret = (char *)__alloca((sizeof(char) * i) ? 4 : 5 + string_length(fmt));

  if (!ret) return ("?");

  const auto fmt_len = string_length(fmt);
  const auto res_len = string_length(boolean_expr);

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

bool StringBuilder::Equals(const char *lhs, const char *rhs) {
  if (string_length(rhs) != string_length(lhs)) return false;

  for (Size index = 0; index < string_length(rhs); ++index) {
    if (rhs[index] != lhs[index]) return false;
  }

  return true;
}

const char *StringBuilder::Format(const char *fmt, const char *fmt2) {
  if (!fmt || !fmt2) return ("?");

  char *ret =
      (char *)alloca(sizeof(char) * string_length(fmt2) + string_length(fmt2));

  if (!ret) return ("?");

  for (Size idx = 0; idx < string_length(fmt); ++idx) {
    if (fmt[idx] == '%') {
      Size result_cnt = idx;
      for (Size y_idx = 0; y_idx < string_length(fmt2); ++y_idx) {
        ret[result_cnt] = fmt2[y_idx];
        ++result_cnt;
      }

      break;
    }

    ret[idx] = fmt[idx];
  }

  return ret;
}

static void string_append(char *lhs, char *rhs, int cur) {
  if (lhs && rhs && cur < string_length(lhs)) {
    SizeT sz_rhs = string_length(rhs);

    rt_copy_memory(rhs, lhs + cur, sz_rhs);
  }
}

StringView &StringView::operator+=(const Char *rhs) {
  if (string_length(rhs) > string_length(this->m_Data)) return *this;

  string_append(this->m_Data, const_cast<char *>(rhs), this->m_Cur);
  this->m_Cur += string_length(rhs);

  return *this;
}

StringView &StringView::operator+=(const StringView &rhs) {
  if (string_length(rhs.m_Data) > string_length(this->m_Data)) return *this;

  string_append(this->m_Data, const_cast<char *>(rhs.m_Data), this->m_Cur);
  this->m_Cur += string_length(const_cast<char *>(rhs.m_Data));

  return *this;
}
}  // namespace HCore
