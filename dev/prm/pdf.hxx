/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <CompilerKit/Detail.hxx>
#include <NewKit/Defines.hxx>

namespace Kernel::PDF
{
	class IPDFRenderViewport;
	class IPDFDocument;
	class IPDFTimestamp;
	class IPDFTag;

	/// \brief PDF rendering class.
	class IPDFRenderViewport
	{
	public:
		explicit IPDFRenderViewport() = default;
		virtual ~IPDFRenderViewport() = default;

		ZKA_COPY_DELETE(IPDFRenderViewport);

		virtual IPDFDocument* GetFirstPage() = 0;
		virtual IPDFDocument* GetLastPage()	 = 0;

		virtual IPDFDocument* GetPage(const UInt32& page_number);

		virtual Void SetProducer(const Char*) = 0;
		virtual Void SetTitle(const Char*)	  = 0;
		virtual Void SetAuthor(const Char*)	  = 0;
		virtual Void SetSubject(const Char*)  = 0;

		virtual const Char* GetProducer() = 0;
		virtual const Char* GetTitle()	  = 0;
		virtual const Char* GetAuthor()	  = 0;
		virtual const Char* GetSubject()  = 0;

		virtual Bool IsFastView() = 0;

		virtual SizeT GetWidth()  = 0;
		virtual SizeT GetHeight() = 0;

		virtual SizeT GetPageCount() = 0;
	};
} // namespace Kernel::PDF
