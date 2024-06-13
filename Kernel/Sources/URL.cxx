/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <CFKit/URL.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Utils.hpp>

/// BUGS: 0

namespace NewOS
{
	URL::URL(StringView& strUrl)
		: fUrlView(strUrl, false)
	{
	}

	URL::~URL() = default;

	/// @brief internal and reserved protocols by kernel.
	constexpr const char* kURLProtocols[] = {
		"file",	 // Filesystem protocol
		"mup",	 // Mahrouss update protocol
		"param", // Mahrouss parameter protocol.
	};

	constexpr const int kUrlOutSz	 = 1; //! such as: ://
	constexpr const int kProtosCount = 3;
	constexpr const int kRangeSz	 = 4096;

	ErrorOr<StringView> url_extract_location(const char* url)
	{
		if (!url || *url == 0 || rt_string_len(url, kRangeSz) > kRangeSz)
			return ErrorOr<StringView>{-1};

		StringView view(rt_string_len(url));

		SizeT i			   = 0;
		bool  scheme_found = false;

		for (; i < rt_string_len(url); ++i)
		{
			if (!scheme_found)
			{
				for (int y = 0; kProtosCount; ++y)
				{
					if (rt_string_in_string(view.CData(), kURLProtocols[y]))
					{
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

	ErrorOr<StringView> url_extract_protocol(const char* url)
	{
		if (!url || *url == 0 || rt_string_len(url, kRangeSz) > kRangeSz)
			return ErrorOr<StringView>{-1};

		ErrorOr<StringView> view{-1};

		return view;
	}

	Ref<ErrorOr<StringView>> URL::Location() noexcept
	{
		const char* src = fUrlView.Leak().CData();
		auto		loc = url_extract_location(src);

		if (!loc)
			return {};

		return Ref<ErrorOr<StringView>>(loc);
	}

	Ref<ErrorOr<StringView>> URL::Protocol() noexcept
	{
		const char* src = fUrlView.Leak().CData();
		auto		loc = url_extract_protocol(src);

		if (!loc)
			return {};

		return Ref<ErrorOr<StringView>>(loc);
	}
} // namespace NewOS
