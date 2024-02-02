/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/PageManager.hpp>
#include <NewKit/UserHeap.hpp>

/// @file Heap.cxx
/// @brief Heap Manager, Process heap allocator.
/// @note if you want to look at the kernel allocator, please look for KernelHeap.cxx
/// bugs: 0

namespace HCore {
/**
 * @brief Heap Manager class, takes care of allocating the process pools.
 * @note This rely on Virtual Memory! Consider adding good vmem support when
 * @note porting to a new arch.
 */
class HeapManager final {
 public:
  STATIC SizeT& Count() { return s_NumPools; }
  STATIC Ref<Pmm>& Leak() { return s_Pmm; }
  STATIC Boolean& IsEnabled() { return s_PoolsAreEnabled; }
  STATIC Array<Ref<PTEWrapper*>, kPoolMaxSz>& The() { return s_Pool; }

 private:
  STATIC Size s_NumPools;
  STATIC Ref<Pmm> s_Pmm;

 private:
  STATIC Boolean s_PoolsAreEnabled;
  STATIC Array<Ref<PTEWrapper*>, kPoolMaxSz> s_Pool;
};

//! declare fields

SizeT HeapManager::s_NumPools = 0UL;
Ref<Pmm> HeapManager::s_Pmm;
Boolean HeapManager::s_PoolsAreEnabled = true;
Array<Ref<PTEWrapper*>, kPoolMaxSz> HeapManager::s_Pool;

STATIC voidPtr ke_find_unused_heap(Int flags);
STATIC void ke_free_heap_internal(voidPtr vaddr);
STATIC voidPtr ke_make_heap(voidPtr vaddr, Int flags);
STATIC Boolean ke_check_and_free_heap(const SizeT& index, voidPtr ptr);

STATIC voidPtr ke_find_unused_heap(Int flags) {
  for (SizeT index = 0; index < kPoolMaxSz; ++index) {
    if (HeapManager::The()[index] &&
        !HeapManager::The()[index].Leak().Leak().Leak()->Present()) {
      HeapManager::Leak().Leak().TogglePresent(
          HeapManager::The()[index].Leak().Leak(), true);
      kcout << "[ke_find_unused_heap] Done, trying now to make a pool\r\n";

      return ke_make_heap((voidPtr)HeapManager::The()[index]
                              .Leak()
                              .Leak()
                              .Leak()
                              ->VirtualAddress(),
                          flags);
    }
  }

  return nullptr;
}

STATIC voidPtr ke_make_heap(voidPtr virtualAddress, Int flags) {
  if (virtualAddress) {
    HeapHeader* pool_hdr = reinterpret_cast<HeapHeader*>(virtualAddress);

    if (!pool_hdr->Free) {
      kcout << "[ke_make_heap] pool_hdr->Free, Pool already exists\n";
      return nullptr;
    }

    pool_hdr->Flags = flags;
    pool_hdr->Magic = kPoolMag;
    pool_hdr->Free = false;

    kcout << "[ke_make_heap] New allocation has been done.\n";
    return reinterpret_cast<voidPtr>(
        (reinterpret_cast<UIntPtr>(virtualAddress) + sizeof(HeapHeader)));
  }

  kcout << "[ke_make_heap] Address is invalid";
  return nullptr;
}

STATIC void ke_free_heap_internal(voidPtr virtualAddress) {
  HeapHeader* pool_hdr = reinterpret_cast<HeapHeader*>(
      reinterpret_cast<UIntPtr>(virtualAddress) - sizeof(HeapHeader));

  if (pool_hdr->Magic == kPoolMag) {
    pool_hdr->Free = false;
    pool_hdr->Flags = 0;

    kcout << "[ke_free_heap_internal] Successfully marked header as free!\r\n";
  }
}

/**
 * @brief Check for the ptr and frees it.
 * 
 * @param index Where to look at.
 * @param ptr The ptr to check.
 * @return Boolean true if successful. 
 */
STATIC Boolean ke_check_and_free_heap(const SizeT& index, voidPtr ptr) {
  if (HeapManager::The()[index]) {
    // ErrorOr<>::operator Boolean
    /// if address matches
    /// -> Free Pool.
    if (HeapManager::The()[index].Leak().Leak().Leak()->VirtualAddress() ==
        (UIntPtr)ptr) {
      HeapManager::Leak().Leak().FreePage(
          HeapManager::The()[index].Leak().Leak());

      --HeapManager::Count();

      ke_free_heap_internal(ptr);
      ptr = nullptr;

      return true;
    }
  }

  return false;
}

/// @brief Creates a new pool pointer.
/// @param flags the flags attached to it.
/// @return a pool pointer with selected permissions.
voidPtr ke_new_heap(Int32 flags) {
  if (!HeapManager::IsEnabled()) return nullptr;

  if (HeapManager::Count() > kPoolMaxSz) return nullptr;

  if (voidPtr ret = ke_find_unused_heap(flags)) return ret;

  // this wasn't set to true
  auto ref_page = HeapManager::Leak().Leak().RequestPage(
      ((flags & kPoolUser)), (flags & kPoolRw));

  if (ref_page) {
    ///! reserve page.
    HeapManager::The()[HeapManager::Count()].Leak() = ref_page;
    auto& ref = HeapManager::Count();

    ++ref;  // increment the number of addresses we have now.

    // finally make the pool address.
    return ke_make_heap(
        reinterpret_cast<voidPtr>(ref_page.Leak()->VirtualAddress()), flags);
  }

  return nullptr;
}

/// @brief free a pool pointer.
/// @param ptr The pool pointer to free.
/// @return status code
Int32 ke_free_heap(voidPtr ptr) {
  if (!HeapManager::IsEnabled()) return -1;

  if (ptr) {
    SizeT base = HeapManager::Count();

    if (ke_check_and_free_heap(base, ptr)) return 0;

    for (SizeT index = 0; index < kPoolMaxSz; ++index) {
      if (ke_check_and_free_heap(index, ptr)) return 0;

      --base;
    }
  }

  return -1;
}

/// @brief Init HeapManager, set Count to zero and IsEnabled to true.
/// @return
Void ke_init_heap() {
  HeapManager::Count() = 0UL;
  HeapManager::IsEnabled() = true;
}
}  // namespace HCore
