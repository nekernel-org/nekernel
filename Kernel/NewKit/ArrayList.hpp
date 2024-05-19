/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

namespace NewOS
{
	template <typename T>
	class ArrayList final
	{
	public:
		explicit ArrayList(T* list)
			: fList(reinterpret_cast<T>(list))
		{
		}

		~ArrayList() = default;

		ArrayList& operator=(const ArrayList&) = default;
		ArrayList(const ArrayList&)			   = default;

		T* Data()
		{
			return fList;
		}

		const T* CData()
		{
			return fList;
		}

		T& operator[](int index) const
		{
			return fList[index];
		}

		operator bool()
		{
			return fList;
		}

	private:
		T* fList;

		friend class InitHelpers;
	};

	template <typename ValueType>
	ArrayList<ValueType> make_list(ValueType val)
	{
		return ArrayList<ValueType>{val};
	}
} // namespace NewOS
