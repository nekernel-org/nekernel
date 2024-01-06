/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

namespace hCore
{
    template <typename T>
    class ArrayList final
    {
    public:
        explicit ArrayList(T *list)
            : m_List(reinterpret_cast<T>(list))
        {}

        ~ArrayList() = default;

        ArrayList &operator=(const ArrayList &) = default;
        ArrayList(const ArrayList &) = default;

        T *Data()
        {
            return m_List;
        }

        const T *CData()
        {
            return m_List;
        }

        T &operator[](int index) const
        {
            return m_List[index];
        }

        operator bool()
        {
            return m_List;
        }

    private:
        T *m_List;

        friend class InitHelpers;

    };

    template <typename ValueType> ArrayList<ValueType> make_list(ValueType val)
    {
        return ArrayList<ValueType>{val};
    }
} // namespace hCore
