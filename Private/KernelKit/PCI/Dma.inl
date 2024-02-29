/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

namespace HCore
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
