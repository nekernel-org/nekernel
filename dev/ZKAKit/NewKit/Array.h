/* -------------------------------------------

	Copyright EL Mahrouss Logic.

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

		T& operator[](const SizeT& At)
		{
			MUST_PASS(At < N);
			return fArray[At];
		}

		T& Assign(const SizeT& At, T& NewVal)
		{
			fArray[At] = NewVal;
			return fArray[At];
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
			SizeT count = 0;

			for (SizeT i = 0; i < N; i++)
			{
				if (fArray[i])
					++count;
			}

			return count;
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
