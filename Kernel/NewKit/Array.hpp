/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */
#pragma once

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/Defines.hpp>

namespace Kernel
{
	template <typename T, Size N>
	class Array final
	{
	public:
		explicit Array() = default;
		~Array()		 = default;

		Array& operator=(const Array&) = default;
		Array(const Array&)			   = default;

		ErrorOr<T*> operator[](Size At)
		{
			if (At > N)
				return {};

			return ErrorOr<T*>(&fArray[At]);
		}

		Boolean Empty() const
		{
			for (auto Val : fArray)
			{
				if (Val)
					return false;
			}

			return true;
		}

		SizeT Count() const
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
