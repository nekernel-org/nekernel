/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/PageManager.hpp>

#define kHeapHeaderPaddingSz (16U)

/// @file UserHeap.cxx
/// @brief User Heap Manager, Process heap allocator.
/// @note if you want to look at the kernel allocator, please look for
/// KernelHeap.cxx
/// BUGS: 0

namespace NewOS {
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
class UserHeapManager final {
 public:
  UserHeapManager() = delete;
  ~UserHeapManager() = default;

 public:
  STATIC SizeT& Count() { return s_NumPools; }
  STATIC Ref<Pmm>& Leak() { return s_Pmm; }
  STATIC Boolean& IsEnabled() { return s_PoolsAreEnabled; }
  STATIC MutableArray<Ref<PTEWrapper>>& The() { return s_Pool; }

 private:
  STATIC Size s_NumPools;
  STATIC Ref<Pmm> s_Pmm;

 private:
  STATIC Boolean s_PoolsAreEnabled;
  STATIC MutableArray<Ref<PTEWrapper>> s_Pool;
};

//! declare fields

SizeT UserHeapManager::s_NumPools = 0UL;
Ref<Pmm> UserHeapManager::s_Pmm;
Boolean UserHeapManager::s_PoolsAreEnabled = true;
MutableArray<Ref<PTEWrapper>> UserHeapManager::s_Pool;

STATIC VoidPtr ke_find_unused_heap(Int32 flags);
STATIC Void ke_free_heap_internal(VoidPtr vaddr);
STATIC VoidPtr ke_make_heap_internal(VoidPtr vaddr, Int32 flags);
STATIC Boolean ke_check_and_free_heap(const SizeT& index, VoidPtr ptr);

/// @brief Find an unused heap header to allocate on.
/// @param flags the flags to use.
/// @return VoidPtr the heap pointer.
STATIC VoidPtr ke_find_unused_heap(Int32 flags) {
  for (SizeT index = 0; index < kUserHeapMaxSz; ++index) {
    if (UserHeapManager::The()[index] &&
        !UserHeapManager::The()[index].Leak().Leak().Present()) {
      UserHeapManager::Leak().Leak().TogglePresent(
          UserHeapManager::The()[index].Leak().Leak(), true);
      kcout << "[ke_find_unused_heap] Done, trying to make a pool now...\r";

      return ke_make_heap_internal(
          (VoidPtr)UserHeapManager::The()[index].Leak().Leak().VirtualAddress(),
          flags);
    }
  }

  return nullptr;
}

/// @brief Makes a new heap for the process to use.
/// @param virtualAddress the virtual address of the process.
/// @param flags the flags.
/// @return
STATIC VoidPtr ke_make_heap_internal(VoidPtr virtualAddress, Int32 flags) {
  if (virtualAddress) {
    UserHeapHeader* poolHdr = reinterpret_cast<UserHeapHeader*>(virtualAddress);

    if (!poolHdr->fFree) {
      kcout
          << "[ke_make_heap_internal] poolHdr->fFree, HeapPtr already exists\n";
      return nullptr;
    }

    poolHdr->fFlags = flags;
    poolHdr->fMagic = kUserHeapMag;
    poolHdr->fFree = false;

    kcout << "[ke_make_heap_internal] New allocation has been done.\n";
    return reinterpret_cast<VoidPtr>(
        (reinterpret_cast<UIntPtr>(virtualAddress) + sizeof(UserHeapHeader)));
  }

  kcout << "[ke_make_heap_internal] Address is invalid";
  return nullptr;
}

/// @brief Internally makrs the heap as free.
/// This is done by setting the fFree bit to true
/// @param virtualAddress
/// @return
STATIC Void ke_free_heap_internal(VoidPtr virtualAddress) {
  UserHeapHeader* poolHdr = reinterpret_cast<UserHeapHeader*>(
      reinterpret_cast<UIntPtr>(virtualAddress) - sizeof(UserHeapHeader));

  if (poolHdr->fMagic == kUserHeapMag) {
    if (!poolHdr->fFree) {
      ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
      return;
    }

    poolHdr->fFree = true;
    poolHdr->fFlags = 0;

    kcout << "[ke_free_heap_internal] Successfully marked header as free!\r";
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
  if (UserHeapManager::The()[index]) {
    // ErrorOr<>::operator Boolean
    /// if (address matches)
    ///     -> Free heap.
    if (UserHeapManager::The()[index].Leak().Leak().VirtualAddress() ==
        (UIntPtr)ptr) {
      UserHeapManager::Leak().Leak().FreePage(
          UserHeapManager::The()[index].Leak().Leak());

      --UserHeapManager::Count();

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
  if (!UserHeapManager::IsEnabled()) return nullptr;

  if (UserHeapManager::Count() > kUserHeapMaxSz) return nullptr;

  if (VoidPtr ret = ke_find_unused_heap(flags)) return ret;

  // this wasn't set to true
  auto ref_page = UserHeapManager::Leak().Leak().RequestPage(
      ((flags & kUserHeapUser)), (flags & kUserHeapRw));

  if (ref_page) {
    ///! reserve page.
    UserHeapManager::The()[UserHeapManager::Count()].Leak() = ref_page;
    auto& ref = UserHeapManager::Count();

    ++ref;  // increment the number of addresses we have now.

    // finally make the pool address.
    return ke_make_heap_internal(
        reinterpret_cast<VoidPtr>(ref_page.Leak().VirtualAddress()), flags);
  }

  return nullptr;
}

/// @brief free a pool pointer.
/// @param ptr The pool pointer to free.
/// @return status code
Int32 rt_free_heap(VoidPtr ptr) {
  if (!UserHeapManager::IsEnabled()) return -1;

  if (ptr) {
    SizeT base = UserHeapManager::Count();

    if (ke_check_and_free_heap(base, ptr)) return 0;

    for (SizeT index = 0; index < kUserHeapMaxSz; ++index) {
      if (ke_check_and_free_heap(index, ptr)) return 0;

      --base;
    }
  }

  return -1;
}
}  // namespace NewOS
