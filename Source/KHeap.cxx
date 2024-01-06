/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/KHeap.hpp>

//! @file KHeap.cpp
//! @brief this allocator is used by kernel to allocate pages.

#define kMaxWrappers (4096 * 8)

namespace hCore
{
    static Ref<PTEWrapper*> kWrapperList[kMaxWrappers];
    static SizeT kWrapperCount = 0UL;
    static Ref<PTEWrapper*> kLastWrapper;
    static Pmm kPmm;

    namespace Detail
    {
        static voidPtr try_find_ptr(const SizeT &sz, const bool rw, const bool user)
        {
            for (SizeT indexWrapper = 0; indexWrapper < kMaxWrappers; ++indexWrapper)
            {
                if (!kWrapperList[indexWrapper]->Present())
                {
                    kWrapperList[indexWrapper]->Reclaim(); /* very straight-forward as you can see. */
                    return reinterpret_cast<voidPtr>(kWrapperList[indexWrapper]->VirtualAddress());
                }
            }

            return nullptr;
        }
    } // namespace Detail

    /// @brief manual allocation
    /// @param sz size of pointer
    /// @param rw read write (true to enable it)
    /// @param user is it accesible by user processes?
    /// @return the pointer
    VoidPtr kernel_new_ptr(const SizeT& sz, const bool rw, const bool user)
    {
        if (kWrapperCount < sz)
            return nullptr;

        if (auto ptr = Detail::try_find_ptr(sz, rw, user);
            ptr)
            return ptr;

        Ref<PTEWrapper*> wrapper = kPmm.RequestPage(user, rw);

        if (wrapper)
        {
            kLastWrapper = wrapper;

            kWrapperList[kWrapperCount] = wrapper;
            ++kWrapperCount;

            return reinterpret_cast<voidPtr>(wrapper->VirtualAddress());
        }

        return nullptr;
    }

    /// @brief Declare pointer as free.
    /// @param ptr the pointer.
    /// @return 
    Int32 kernel_delete_ptr(voidPtr ptr)
    {
        if (ptr)
        {
            const UIntPtr virtualAddress = reinterpret_cast<UIntPtr>(ptr);

            if (kLastWrapper &&
            virtualAddress == kLastWrapper->VirtualAddress())
            {
                return kPmm.FreePage(kLastWrapper);
            }

            Ref<PTEWrapper*> wrapper;

            for (SizeT indexWrapper = 0; indexWrapper < kWrapperCount; ++indexWrapper)
            {
                if (kWrapperList[indexWrapper]->VirtualAddress() == virtualAddress)
                {
                    wrapper = kWrapperList[indexWrapper];
                    return kPmm.FreePage(wrapper);
                }
            }
        }

        return -1;
    }

    /// @brief find pointer in kernel heap
    /// @param ptr the pointer
    /// @return if it exists.
    Boolean kernel_valid_ptr(voidPtr ptr)
    {
        if (ptr)
        {
            const UIntPtr virtualAddress = reinterpret_cast<UIntPtr>(ptr);

            if (kLastWrapper &&
                virtualAddress == kLastWrapper->VirtualAddress())
            {
                return true;
            }

            Ref<PTEWrapper*> wrapper;

            for (SizeT indexWrapper = 0; indexWrapper < kWrapperCount; ++indexWrapper)
            {
                if (kWrapperList[indexWrapper]->VirtualAddress() == virtualAddress)
                {
                    wrapper = kWrapperList[indexWrapper];
                    return true;
                }
            }
        }

        return false;
    }
} // namespace hCore
