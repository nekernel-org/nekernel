/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/PageManager.hpp>
#include <NewKit/UserHeap.hpp>

/// @file Heap.cxx
/// @brief Heap Manager, user mode allocator.
/// @note if you want to look at kernel allocs, please look for KHeap.cxx
/// bugs: 0

namespace HCore {
class HeapManager final {
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

static voidPtr ke_find_unused_heap(Int flags);
static void ke_free_heap_internal(voidPtr vaddr);
static voidPtr ke_make_heap(voidPtr vaddr, Int flags);
static bool ke_check_and_free_heap(const SizeT& index, voidPtr ptr);

static voidPtr ke_find_unused_heap(Int flags) {
  for (SizeT index = 0; index < kPoolMaxSz; ++index) {
    if (HeapManager::The()[index] &&
        !HeapManager::The()[index].Leak().Leak().Leak()->Present()) {
      HeapManager::GetPmm().Leak().TogglePresent(
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

static voidPtr ke_make_heap(voidPtr virtualAddress, Int flags) {
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

static void ke_free_heap_internal(voidPtr virtualAddress) {
  HeapHeader* pool_hdr = reinterpret_cast<HeapHeader*>(
      reinterpret_cast<UIntPtr>(virtualAddress) - sizeof(HeapHeader));

  if (pool_hdr->Magic == kPoolMag) {
    pool_hdr->Free = false;
    pool_hdr->Flags = 0;

    kcout << "[ke_free_heap_internal] Successfully marked header as free!\r\n";
  }
}

static bool ke_check_and_free_heap(const SizeT& index, voidPtr ptr) {
  if (HeapManager::The()[index]) {
    // ErrorOr<>::operator bool
    if (!HeapManager::The()[index].Leak().Leak().IsStrong()) {
      // we want them to be weak
      // because we allocated it.
      if (HeapManager::The()[index].Leak().Leak().Leak()->VirtualAddress() ==
          (UIntPtr)ptr) {
        HeapManager::GetPmm().Leak().FreePage(
            HeapManager::The()[index].Leak().Leak());
        --HeapManager::GetCount();

        ke_free_heap_internal(ptr);
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
voidPtr ke_new_heap(Int32 flags) {
  if (!HeapManager::IsEnabled()) return nullptr;

  if (HeapManager::GetCount() > kPoolMaxSz) return nullptr;

  if (voidPtr ret = ke_find_unused_heap(flags)) return ret;

  // this wasn't set to true
  auto ref_page = HeapManager::GetPmm().Leak().RequestPage(
      ((flags & kPoolUser)), (flags & kPoolRw));
  if (ref_page) {
    ///! reserve page.
    HeapManager::The()[HeapManager::GetCount()].Leak() = ref_page;
    auto& ref = HeapManager::GetCount();
    ++ref;  // increment the number of addresses we have now.

    kcout << "[ke_new_heap] New Address found!\r\n";

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
    SizeT base = HeapManager::GetCount();

    if (ke_check_and_free_heap(base, ptr)) return 0;

    for (SizeT index = 0; index < kPoolMaxSz; ++index) {
      if (ke_check_and_free_heap(index, ptr)) return 0;

      --base;
    }
  }

  return -1;
}

/// @brief Init HeapManager, set GetCount to zero and IsEnabled to true.
/// @return 
Void ke_init_heap() {
  HeapManager::GetCount() = 0UL;
  HeapManager::IsEnabled() = true;
}
}  // namespace HCore
