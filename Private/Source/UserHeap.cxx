/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <NewKit/PageManager.hpp>
#include <KernelKit/UserHeap.hpp>

#define kHeapHeaderPaddingSz 16

/// @file UserHeap.cxx
/// @brief User Heap Manager, Process heap allocator.
/// @note if you want to look at the kernel allocator, please look for
/// KernelHeap.cxx
/// BUGS: 0

namespace HCore {
/**
 * @brief Process Heap Header
 * @note Allocated per process, it denotes the user's heap.
*/
struct UserHeapHeader final {
  UInt32 fMagic;
  Int32 fFlags;
  Boolean fFree;
  UInt8 fPadding[kHeapHeaderPaddingSz];
};

/**
 * @brief User Heap Manager class, takes care of allocating the process pools.
 * @note This rely on Virtual Memory! Consider adding good vmem support when
 * @note porting to a new arch.
 */
class HeapManager final {
 public:
  STATIC SizeT& Count() { return s_NumPools; }
  STATIC Ref<Pmm>& Leak() { return s_Pmm; }
  STATIC Boolean& IsEnabled() { return s_PoolsAreEnabled; }
  STATIC Array<Ref<PTEWrapper>, kUserHeapMaxSz>& The() { return s_Pool; }

 private:
  STATIC Size s_NumPools;
  STATIC Ref<Pmm> s_Pmm;

 private:
  STATIC Boolean s_PoolsAreEnabled;
  STATIC Array<Ref<PTEWrapper>, kUserHeapMaxSz> s_Pool;
};

//! declare fields

SizeT HeapManager::s_NumPools = 0UL;
Ref<Pmm> HeapManager::s_Pmm;
Boolean HeapManager::s_PoolsAreEnabled = true;
Array<Ref<PTEWrapper>, kUserHeapMaxSz> HeapManager::s_Pool;

STATIC VoidPtr ke_find_unused_heap(Int flags);
STATIC Void ke_free_heap_internal(VoidPtr vaddr);
STATIC VoidPtr ke_make_heap(VoidPtr vaddr, Int flags);
STATIC Boolean ke_check_and_free_heap(const SizeT& index, VoidPtr ptr);

STATIC VoidPtr ke_find_unused_heap(Int flags) {
  for (SizeT index = 0; index < kUserHeapMaxSz; ++index) {
    if (HeapManager::The()[index] &&
        !HeapManager::The()[index].Leak().Leak().Leak().Present()) {
      HeapManager::Leak().Leak().TogglePresent(
          HeapManager::The()[index].Leak().Leak(), true);
      kcout << "[ke_find_unused_heap] Done, trying to make a pool now...\r\n";

      return ke_make_heap((VoidPtr)HeapManager::The()[index]
                              .Leak()
                              .Leak()
                              .Leak()
                              .VirtualAddress(),
                          flags);
    }
  }

  return nullptr;
}

STATIC VoidPtr ke_make_heap(VoidPtr virtualAddress, Int flags) {
  if (virtualAddress) {
    UserHeapHeader* poolHdr = reinterpret_cast<UserHeapHeader*>(virtualAddress);

    if (!poolHdr->fFree) {
      kcout << "[ke_make_heap] poolHdr->fFree, HeapPtr already exists\n";
      return nullptr;
    }

    poolHdr->fFlags = flags;
    poolHdr->fMagic = kUserHeapMag;
    poolHdr->fFree = false;

    kcout << "[ke_make_heap] New allocation has been done.\n";
    return reinterpret_cast<VoidPtr>(
        (reinterpret_cast<UIntPtr>(virtualAddress) + sizeof(UserHeapHeader)));
  }

  kcout << "[ke_make_heap] Address is invalid";
  return nullptr;
}

STATIC Void ke_free_heap_internal(VoidPtr virtualAddress) {
  UserHeapHeader* poolHdr = reinterpret_cast<UserHeapHeader*>(
      reinterpret_cast<UIntPtr>(virtualAddress) - sizeof(UserHeapHeader));

  if (poolHdr->fMagic == kUserHeapMag) {
    MUST_PASS(poolHdr->fFree);

    poolHdr->fFree = false;
    poolHdr->fFlags = 0;

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
STATIC Boolean ke_check_and_free_heap(const SizeT& index, VoidPtr ptr) {
  if (HeapManager::The()[index]) {
    // ErrorOr<>::operator Boolean
    /// if (address matches)
    ///     -> Free heap.
    if (HeapManager::The()[index].Leak().Leak().Leak().VirtualAddress() ==
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
VoidPtr rt_new_heap(Int32 flags) {
  if (!HeapManager::IsEnabled()) return nullptr;

  if (HeapManager::Count() > kUserHeapMaxSz) return nullptr;

  if (VoidPtr ret = ke_find_unused_heap(flags)) return ret;

  // this wasn't set to true
  auto ref_page = HeapManager::Leak().Leak().RequestPage(((flags & kUserHeapUser)),
                                                         (flags & kUserHeapRw));

  if (ref_page) {
    ///! reserve page.
    HeapManager::The()[HeapManager::Count()].Leak() = ref_page;
    auto& ref = HeapManager::Count();

    ++ref;  // increment the number of addresses we have now.

    // finally make the pool address.
    return ke_make_heap(
        reinterpret_cast<VoidPtr>(ref_page.Leak().VirtualAddress()), flags);
  }

  return nullptr;
}

/// @brief free a pool pointer.
/// @param ptr The pool pointer to free.
/// @return status code
Int32 rt_free_heap(VoidPtr ptr) {
  if (!HeapManager::IsEnabled()) return -1;

  if (ptr) {
    SizeT base = HeapManager::Count();

    if (ke_check_and_free_heap(base, ptr)) return 0;

    for (SizeT index = 0; index < kUserHeapMaxSz; ++index) {
      if (ke_check_and_free_heap(index, ptr)) return 0;

      --base;
    }
  }

  return -1;
}
}  // namespace HCore
