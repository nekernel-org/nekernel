/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <SCIKit/SCI.h>

namespace LibCF
{
	template <typename T, SizeT N>
	class CFArray final
	{
	public:
		explicit CFArray() = default;
		~CFArray()		   = default;

		CFArray& operator=(const CFArray&) = default;
		CFArray(const CFArray&)			   = default;

		T& operator[](const SizeT& at)
		{
			MUST_PASS(at < this->Count());
			return fArray[at];
		}

		Bool Empty()
		{
			return this->Count() > 0;
		}

		const SizeT Capacity()
		{
			return N;
		}

		const SizeT Count()
		{
			auto cnt = 0UL;

			for (auto i = 0; i < N; ++i)
			{
				if (fArray[i])
					++cnt;
			}

			return cnt;
		}

		const T* CData()
		{
			return fArray;
		}

		operator bool()
		{
			return !Empty();
		}

	private:
		T fArray[N] = {nullptr};
	};

	template <typename ValueType>
	auto make_array(ValueType val)
	{
		return CFArray<ValueType, ARRAY_SIZE(val)>{val};
	}
} // namespace LibCF
