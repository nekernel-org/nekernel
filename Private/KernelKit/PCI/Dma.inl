/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

namespace hCore
{
    template<class T>
    T* DMAWrapper::operator->()
    {
        return m_Address;
    }

    template<class T>
    T* DMAWrapper::Get(const UIntPtr offset)
    {
        return reinterpret_cast<T*>((UIntPtr) m_Address + offset);
    }
}
