/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */
#pragma once

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/Defines.hpp>

namespace HCore
{
template <typename T, Size N> 
class Array final
{
public:
    Array() {}
    ~Array() {}

    Array &operator=(const Array &) = default;
    Array(const Array &) = default;

    ErrorOr<T> operator[](Size At)
    {
        if (At > N)
            return {};

        kcout << "Returning element\r\n";
        return ErrorOr<T>(m_Array[At]);
    }

    Boolean Empty() const
    {
        for (auto Val : m_Array)
        {
            if (Val)
                return false;
        }

        return true;
    }

    const T *CData()
    {
        return m_Array;
    }

    operator bool()
    {
        return !Empty();
    }

private:
    T m_Array[N];
    
};
} // namespace HCore
