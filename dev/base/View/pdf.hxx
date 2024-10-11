/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <sci/sci_base.hxx>

namespace View::PDF
{
	class IPDF;
	class IPDFDocument;
	class IPDFTimestamp;
	class IPDFTag;

	/// \brief PDF manipulator class.
	class IPDF
	{
	public:
		explicit IPDF() = default;
		virtual ~IPDF() = default;

		IPDF& operator=(const IPDF&) = delete;
		IPDF(const IPDF&)			 = delete;

		virtual IPDFDocument* GetFirstPage() = 0;
		virtual IPDFDocument* GetLastPage()	 = 0;

		virtual IPDFDocument* GetPage(const UInt32& page_number);
		virtual SInt32		  SetPage(const UInt32& page_number, IPDFDocument* new_page);

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
} // namespace View::PDF
