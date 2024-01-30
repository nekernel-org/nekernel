/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_URL_HPP__
#define _INC_URL_HPP__

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

namespace HCore {
class Url final {
 public:
  explicit Url(StringView &strUrl);
  ~Url();

 public:
  Ref<ErrorOr<StringView>> Location() noexcept;
  Ref<ErrorOr<StringView>> Protocol() noexcept;

 private:
  Ref<StringView> m_urlView;
};

}  // namespace HCore

#endif /* ifndef _INC_URL_HPP__ */
