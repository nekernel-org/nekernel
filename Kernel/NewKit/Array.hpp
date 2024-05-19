/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */
#pragma once

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/Defines.hpp>

namespace NewOS
{
	template <typename T, Size N>
	class Array final
	{
	public:
		explicit Array() = default;
		~Array()		 = default;

		Array& operator=(const Array&) = default;
		Array(const Array&)			   = default;

		ErrorOr<T> operator[](Size At)
		{
			if (At > N)
				return {};

			kcout << "Returning element\r";
			return ErrorOr<T>(fArray[At]);
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
			SizeT cntElems = 0UL;
			for (auto Val : fArray)
			{
				if (Val)
					++cntElems;
			}

			return cntElems;
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
} // namespace NewOS
