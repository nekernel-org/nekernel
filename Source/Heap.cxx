/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/Heap.hpp>
#include <NewKit/PageManager.hpp>

/// @file Heap.cxx
/// @brief hCore Process Heap Manager
/// @note if you want to look at kernel allocs, please look for KHeap.cxx
/// bugs: 0

namespace hCore
{
    class HeapManager final
    {
    public:
        static SizeT& GetCount() { return s_NumPools; }
        static Ref<Pmm>& GetPmm() { return s_Pmm; }
        static Boolean& IsEnabled() { return s_PoolsAreEnabled; }
        static Array<Ref<PTEWrapper*>, kPoolMaxSz>& The() { return s_Pool; }

    private:
        static Size s_NumPools;
        static Ref<Pmm> s_Pmm;

    private:
        static Boolean s_PoolsAreEnabled;
        static Array<Ref<PTEWrapper*>, kPoolMaxSz> s_Pool;

    };

    //! declare fields

    SizeT HeapManager::s_NumPools = 0UL;
    Ref<Pmm> HeapManager::s_Pmm;
    Boolean HeapManager::s_PoolsAreEnabled = true;
    Array<Ref<PTEWrapper*>, kPoolMaxSz> HeapManager::s_Pool;

    static voidPtr rt_find_unused_heap(Int flags);
    static void rt_free_heap_internal(voidPtr vaddr);
    static voidPtr rt_make_heap(voidPtr vaddr, Int flags);
    static bool rt_check_and_free_heap(const SizeT &index, voidPtr ptr);

    static voidPtr rt_find_unused_heap(Int flags)
    {
        for (SizeT index = 0; index < kPoolMaxSz; ++index)
        {
            if (HeapManager::The()[index] && 
                !HeapManager::The()[index].Leak().Leak().Leak()->Present())
            {
                HeapManager::GetPmm().Leak().TogglePresent(HeapManager::The()[index].Leak().Leak(), true);
                kcout << "[rt_find_unused_heap] Done, trying now to make a pool\r\n";

                return rt_make_heap((voidPtr)HeapManager::The()[index].Leak().Leak().Leak()->VirtualAddress(), flags);
            }
        }

        return nullptr;
    }

    static voidPtr rt_make_heap(voidPtr virtualAddress, Int flags)
    {
        if (virtualAddress)
        {
            HeapHeader* pool_hdr = reinterpret_cast<HeapHeader*>(virtualAddress);

            if (!pool_hdr->Free)
            {
                kcout << "[rt_make_heap] pool_hdr->Free, Pool already exists\n";
                return nullptr;
            }

            pool_hdr->Flags = flags;
            pool_hdr->Magic = kPoolMag;
            pool_hdr->Free = false;

            kcout << "[rt_make_heap] New allocation has been done.\n";
            return reinterpret_cast<voidPtr>((reinterpret_cast<UIntPtr>(virtualAddress) + sizeof(HeapHeader)));
        }

        kcout << "[rt_make_heap] Address is invalid";
        return nullptr;
    }

    static void rt_free_heap_internal(voidPtr virtualAddress)
    {
        HeapHeader* pool_hdr = reinterpret_cast<HeapHeader*>(reinterpret_cast<UIntPtr>(virtualAddress) - sizeof(HeapHeader));

        if (pool_hdr->Magic == kPoolMag)
        {
            pool_hdr->Free = false;
            pool_hdr->Flags = 0;

            kcout << "[rt_free_heap_internal] Successfully marked header as free!\r\n";
        }
    }

    static bool rt_check_and_free_heap(const SizeT& index, voidPtr ptr)
    {
        if (HeapManager::The()[index])
        { 
            // ErrorOr<>::operator bool
            if (!HeapManager::The()[index].Leak().Leak().IsStrong())
            { 
            	// we want them to be weak
                // because we allocated it.
                if (HeapManager::The()[index].Leak().Leak().Leak()->VirtualAddress() == (UIntPtr) ptr)
                {
                    HeapManager::GetPmm().Leak().FreePage(HeapManager::The()[index].Leak().Leak());
                    --HeapManager::GetCount();

                    rt_free_heap_internal(ptr);
                    ptr = nullptr;

                    return true;
                }
            }
        }

        return false;
    }

    /// @brief Creates a new pool pointer.
    /// @param flags the flags attached to it.
    /// @return a pool pointer with selected permissions.
    voidPtr pool_new_ptr(Int32 flags)
    {
        if (!HeapManager::IsEnabled())
            return nullptr;

        if (HeapManager::GetCount() > kPoolMaxSz)
            return nullptr;

        if (voidPtr ret = rt_find_unused_heap(flags))
            return ret;

        // this wasn't set to true
        auto ref_page = HeapManager::GetPmm().Leak().RequestPage(((flags & kPoolUser)),
                                                         (flags & kPoolRw));
        if (ref_page)
        {
            ///! reserve page.
            HeapManager::The()[HeapManager::GetCount()].Leak() = ref_page;
            auto& ref = HeapManager::GetCount();
            ++ref; // increment the number of addresses we have now.

            kcout << "[pool_new_ptr] New Address found!\r\n";

            // finally make the pool address.
            return rt_make_heap(reinterpret_cast<voidPtr>(ref_page.Leak()->VirtualAddress()), flags);
        }

        return nullptr;
    }

    /// @brief free a pool pointer.
    /// @param ptr The pool pointer to free.
    /// @return status code
    Int32 pool_free_ptr(voidPtr ptr)
    {
        if (!HeapManager::IsEnabled())
            return -1;

        if (ptr)
        {
            SizeT base = HeapManager::GetCount();

            if (rt_check_and_free_heap(base, ptr))
                return 0;

            for (SizeT index = 0; index < kPoolMaxSz; ++index)
            {
                if (rt_check_and_free_heap(index, ptr))
                    return 0;

                --base;
            }
        }

        return -1;
    }

    /// @brief Checks if pointer is valid.
    /// @param thePool the pool pointer.
    /// @param thePtr the pointer.
    /// @param theLimit the last address of the pool.
    /// @return if it is valid.
    Boolean pool_ptr_exists(UIntPtr thePool, UIntPtr thePtr, SizeT theLimit)
    {
        if (HeapManager::GetCount() < 1)
            return false;

        if (thePool == 0 ||
            thePtr == 0 ||
            theLimit == 0)
        {
            return false;
        }

        return thePool < thePtr < (theLimit);
    }
} // namespace hCore
