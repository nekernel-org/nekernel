/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef _INC_URL_HPP_
#define _INC_URL_HPP_

#include <NewKit/Defines.hxx>
#include <NewKit/String.hxx>

namespace Kernel
{
	class URL final
	{
	public:
		explicit URL(StringView& strUrl);
		~URL();

	public:
		Ref<ErrorOr<StringView>> Location() noexcept;
		Ref<ErrorOr<StringView>> Protocol() noexcept;

	private:
		Ref<StringView> fUrlView;
	};

	ErrorOr<StringView> url_extract_location(const Char* url);
	ErrorOr<StringView> url_extract_protocol(const Char* url);
} // namespace Kernel

#endif /* ifndef _INC_URL_HPP_ */
