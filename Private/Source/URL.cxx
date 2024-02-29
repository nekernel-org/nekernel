/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <CFKit/URL.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Utils.hpp>

// Bugs = 0

namespace HCore {
Url::Url(StringView &strUrl) : m_urlView(strUrl, false) {}

Url::~Url() = default;

constexpr const char *kURLProtocols[] = {
    "https",   // http with the secure.
    "http",    // http without the secure
    "file",    // filesystem protocol
    "ftp",     // file transfer protocol
    "params",  // system settings
    "rsh",     // remote shell (gui)
};

constexpr const int kUrlOutSz = 3;  //! ://
constexpr const int kProtosCount = 8;
constexpr const int kRangeSz = 4096;

static ErrorOr<StringView> url_extract_location(const char *url) {
  if (!url || *url == 0 || rt_string_len(url, kRangeSz) > kRangeSz)
    return ErrorOr<StringView>{-1};

  StringView view(rt_string_len(url));

  SizeT i = 0;
  bool scheme_found = false;

  for (; i < rt_string_len(url); ++i) {
    if (!scheme_found) {
      for (int y = 0; kProtosCount; ++y) {
        if (rt_string_in_string(view.CData(), kURLProtocols[y])) {
          i += rt_string_len(kURLProtocols[y]) + kUrlOutSz;
          scheme_found = true;

          break;
        }
      }
    }

    view.Data()[i] = url[i];
  }

  return ErrorOr<StringView>(view);
}

static ErrorOr<StringView> url_extract_protocol(const char *url) {
  if (!url || *url == 0 || rt_string_len(url, kRangeSz) > kRangeSz)
    return ErrorOr<StringView>{-1};

  ErrorOr<StringView> view{-1};

  return view;
}

Ref<ErrorOr<StringView>> Url::Location() noexcept {
  const char *src = m_urlView.Leak().CData();
  auto loc = url_extract_location(src);

  if (!loc) return {};

  return Ref<ErrorOr<StringView>>(loc);
}

Ref<ErrorOr<StringView>> Url::Protocol() noexcept {
  const char *src = m_urlView.Leak().CData();
  auto loc = url_extract_protocol(src);

  if (!loc) return {};

  return Ref<ErrorOr<StringView>>(loc);
}
}  // namespace HCore
