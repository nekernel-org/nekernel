/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/Defines.h>

namespace Kernel
{
	template <typename T, SizeT N>
	class Array final
	{
	public:
		explicit Array() = default;
		~Array()		 = default;

		Array& operator=(const Array&) = default;
		Array(const Array&)			   = default;

		T& operator[](const SizeT& at)
		{
			return fArray[at];
		}

		Boolean Empty()
		{
			return this->Count() > 0;
		}

		const SizeT Capacity()
		{
			return N;
		}

		const SizeT Count()
		{
			return N;
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
		T fArray[N];
	};
} // namespace Kernel
