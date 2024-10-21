/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/Defines.hxx>

namespace Kernel
{
	template <typename T, SizeT N>
	class Array final
	{
	public:
		explicit Array()
		{
			for (SizeT i = 0; i < N; i++)
			{
				if (!fArray[i])
					fArray[i] = T();
			}
		}

		~Array() = default;

		Array& operator=(const Array&) = default;
		Array(const Array&)			   = default;

		T& operator[](const SizeT& At)
		{
			return fArray[At];
		}

		Boolean Empty() const
		{
			return No;
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
