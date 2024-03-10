/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */
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
    explicit Array() = default;
    ~Array() = default;

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

    SizeT Count() const
    {
        SizeT cntElems = 0UL;
        for (auto Val : m_Array)
        {
            if (Val)
                ++cntElems;
        }

        return cntElems;
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
