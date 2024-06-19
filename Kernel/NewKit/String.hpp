/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/KernelCheck.hpp>

namespace NewOS
{
	class StringView final
	{
	public:
		explicit StringView() = default;

		explicit StringView(Size Sz)
			: fSz(Sz)
		{
			MUST_PASS(Sz > 1);
			fData = new Char[Sz];
			MUST_PASS(fData);
		}

		~StringView()
		{
			if (fData)
				delete[] fData;
		}

		StringView& operator=(const StringView&) = default;
		StringView(const StringView&)			 = default;

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
		Size  fSz{0};
		Size  fCur{0};

		friend class StringBuilder;
	};

	struct StringBuilder final
	{
		static ErrorOr<StringView> Construct(const Char* data);
		static const char*		   FromInt(const char* fmt, int n);
		static const char*		   FromBool(const char* fmt, bool n);
		static const char*		   Format(const char* fmt, const char* from);
		static bool				   Equals(const char* lhs, const char* rhs);
	};
} // namespace NewOS
