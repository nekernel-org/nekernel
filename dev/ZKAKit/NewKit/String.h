/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NewKit/Defines.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/Utils.h>
#include <NewKit/Stop.h>

#define cMinimumStringSize 8196

namespace Kernel
{
	/// @brief StringView class, using dynamic or static memory.
	class StringView final
	{
	public:
		explicit StringView()
		{
			fDataSz = cMinimumStringSize;

			fData = new Char[fDataSz];
			MUST_PASS(fData);

			rt_set_memory(fData, 0, fDataSz);
		}

		explicit StringView(const SizeT& Sz)
			: fDataSz(Sz)
		{
			MUST_PASS(Sz > 1);

			fData = new Char[Sz];
			MUST_PASS(fData);

			rt_set_memory(fData, 0, Sz);
		}

		~StringView()
		{
			if (fData)
				delete[] fData;
		}

		ZKA_COPY_DEFAULT(StringView);

		Char*		Data();
		const Char* CData() const;
		Size		Length() const;

		bool operator==(const Char* rhs) const;
		bool operator!=(const Char* rhs) const;

		bool operator==(const StringView& rhs) const;
		bool operator!=(const StringView& rhs) const;

		StringView& operator+=(const Char* rhs);
		StringView& operator+=(const StringView& rhs);

		operator bool()
		{
			return fData;
		}

		bool operator!()
		{
			return fData;
		}

	private:
		Char* fData{nullptr};
		Size  fDataSz{0};
		Size  fCur{0};

		friend class StringBuilder;
	};

	struct StringBuilder final
	{
		static ErrorOr<StringView> Construct(const Char* data);
		static const Char*		   FromBool(const Char* fmt, bool n);
		static const Char*		   Format(const Char* fmt, const Char* from);
		static bool				   Equals(const Char* lhs, const Char* rhs);
		static bool				   Equals(const WideChar* lhs, const WideChar* rhs);
	};
} // namespace Kernel
