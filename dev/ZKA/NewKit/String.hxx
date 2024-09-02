/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/Utils.hxx>
#include <NewKit/KernelCheck.hxx>

namespace Kernel
{
	class StringView final
	{
	public:
		explicit StringView()
		{
			fSz = 4096;

			fData = new Char[fSz];
			MUST_PASS(fData);

			rt_set_memory(fData, 0, fSz);
		}

		explicit StringView(Size Sz)
			: fSz(Sz)
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
		static const Char*		   FromInt(const Char* fmt, int n);
		static const Char*		   FromBool(const Char* fmt, bool n);
		static const Char*		   Format(const Char* fmt, const Char* from);
		static bool				   Equals(const Char* lhs, const Char* rhs);
		static bool				   Equals(const WideChar* lhs, const WideChar* rhs);
	};
} // namespace Kernel
