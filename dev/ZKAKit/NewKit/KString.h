/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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
	/// @brief KString static string class.
	class KString final
	{
	public:
		explicit KString()
		{
			fDataSz = cMinimumStringSize;

			fData = new Char[fDataSz];
			MUST_PASS(fData);

			rt_set_memory(fData, 0, fDataSz);
		}

		explicit KString(const SizeT& Sz)
			: fDataSz(Sz)
		{
			MUST_PASS(Sz > 1);

			fData = new Char[Sz];
			MUST_PASS(fData);

			rt_set_memory(fData, 0, Sz);
		}

		~KString()
		{
			if (fData)
			{
				delete[] fData;
				fData = nullptr;
			}
		}

		ZKA_COPY_DEFAULT(KString);

		Char*		Data();
		const Char* CData() const;
		Size		Length() const;

		bool operator==(const Char* rhs) const;
		bool operator!=(const Char* rhs) const;

		bool operator==(const KString& rhs) const;
		bool operator!=(const KString& rhs) const;

		KString& operator+=(const Char* rhs);
		KString& operator+=(const KString& rhs);

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
		static ErrorOr<KString> Construct(const Char* data);
		static const Char*		FromBool(const Char* fmt, bool n);
		static const Char*		Format(const Char* fmt, const Char* from);
		static bool				Equals(const Char* lhs, const Char* rhs);
		static bool				Equals(const WideChar* lhs, const WideChar* rhs);
	};
} // namespace Kernel
