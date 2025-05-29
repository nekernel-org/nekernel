/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __NE_AMD64__
#include <HALKit/AMD64/Paging.h>
#elif defined(__NE_ARM64__)
#include <HALKit/ARM64/Paging.h>
#endif

#include <ArchKit/ArchKit.h>
#include <NeKit/KernelPanic.h>

#define kBitMapMagic (0x10210U)

#define kBitMapMagIdx (0U)
#define kBitMapSizeIdx (1U)
#define kBitMapUsedIdx (2U)

namespace Kernel {
namespace HAL {
  namespace Detail {
    STATIC SizeT kBitMapCursor = 0UL;

    /***********************************************************************************/
    /// \brief Proxy Interface to manage a bitmap allocator.
    /***********************************************************************************/
    class IBitMapProxy final {
     public:
      explicit IBitMapProxy() = default;
      ~IBitMapProxy()         = default;

      NE_COPY_DELETE(IBitMapProxy)

      auto IsBitMap(VoidPtr page_ptr) -> Bool {
        if (!page_ptr) return No;

        UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

        if (!ptr_bit_set[kBitMapMagIdx] || ptr_bit_set[kBitMapMagIdx] != kBitMapMagic) return No;

        return Yes;
      }

      auto FreeBitMap(VoidPtr page_ptr) -> Bool {
        if (this->IsBitMap(page_ptr) == No) return No;

        UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

        kBitMapCursor += ptr_bit_set[kBitMapSizeIdx];

        ptr_bit_set[kBitMapMagIdx]  = kBitMapMagic;
        ptr_bit_set[kBitMapUsedIdx] = No;

        this->GetBitMapStatus(ptr_bit_set);

        return Yes;
      }

      UInt32 MakeMMFlags(Bool wr, Bool user) {
        UInt32 flags = kMMFlagsPresent;

        if (wr) flags |= kMMFlagsWr;
        if (user) flags |= kMMFlagsUser;

        return flags;
      }

      /***********************************************************************************/
      /// @brief Iterate over availables bitmap, until we find a free entry.
      /// @param base_ptr base pointer to look on.
      /// @param size the size of the requested data structure.
      /// @param wr is writable flag?
      /// @param user is user flag?
      /// @param pad additional padding added to **size**
      /// @return The new free address, or nullptr.
      /***********************************************************************************/
      auto FindBitMap(VoidPtr base_ptr, SizeT size, Bool wr, Bool user, SizeT pad) -> VoidPtr {
        if (!size) return nullptr;

        if (kBitMapCursor > kKernelBitMpSize) {
          err_global_get() = kErrorOutOfBitMapMemory;

          (Void)(kout << "Bitmap limit reached, can't allocate more bitmaps." << kendl);
          return nullptr;
        }

        VoidPtr base = reinterpret_cast<VoidPtr>((UIntPtr) base_ptr);

        MUST_PASS(base);

        STATIC SizeT biggest = 0UL;

        while (YES) {
          UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base);

          if (ptr_bit_set[kBitMapMagIdx] == kBitMapMagic &&
              ptr_bit_set[kBitMapSizeIdx] == (size + pad)) {
            if (ptr_bit_set[kBitMapUsedIdx] == No) {
              ptr_bit_set[kBitMapSizeIdx] = size + pad;
              ptr_bit_set[kBitMapUsedIdx] = Yes;

              this->GetBitMapStatus(ptr_bit_set);

              UInt32 flags = this->MakeMMFlags(wr, user);
              mm_map_page(ptr_bit_set, ptr_bit_set, flags);

              if (biggest < (size + pad)) biggest = size + pad;

              kBitMapCursor += size + pad;

              return (VoidPtr) ptr_bit_set;
            }
          } else if (ptr_bit_set[kBitMapMagIdx] != kBitMapMagic) {
            ptr_bit_set[kBitMapMagIdx]  = kBitMapMagic;
            ptr_bit_set[kBitMapSizeIdx] = (size + pad);
            ptr_bit_set[kBitMapUsedIdx] = Yes;

            this->GetBitMapStatus(ptr_bit_set);

            UInt32 flags = this->MakeMMFlags(wr, user);
            mm_map_page(ptr_bit_set, ptr_bit_set, flags);

            if (biggest < (size + pad)) biggest = (size + pad);

            kBitMapCursor += size + pad;

            return (VoidPtr) ptr_bit_set;
          }

          UIntPtr raw_base = reinterpret_cast<UIntPtr>(base);
          UIntPtr offset   = (ptr_bit_set[kBitMapMagIdx] != kBitMapMagic)
                                 ? (size + pad)
                                 : ptr_bit_set[kBitMapSizeIdx];

          base = reinterpret_cast<VoidPtr>(raw_base + offset);

          if (base == nullptr) return nullptr;
        }

        return nullptr;
      }

      /// @brief Print Bitmap status
      auto GetBitMapStatus(UIntPtr* ptr_bit_set) -> Void {
        if (!this->IsBitMap(ptr_bit_set)) {
          (Void)(kout << "Not a BitMap: " << hex_number((UIntPtr) ptr_bit_set) << kendl);
          return;
        }

        (Void)(kout << "Magic: " << hex_number(ptr_bit_set[kBitMapMagIdx]) << kendl);
        (Void)(kout << "Is Allocated? " << (ptr_bit_set[kBitMapUsedIdx] ? "YES" : "NO") << kendl);
        (Void)(kout << "Size of BitMap (B): " << number(ptr_bit_set[kBitMapSizeIdx]) << kendl);
        (Void)(kout << "Size of BitMap (KIB): " << number(KIB(ptr_bit_set[kBitMapSizeIdx]))
                    << kendl);
        (Void)(kout << "Size of BitMap (MIB): " << number(MIB(ptr_bit_set[kBitMapSizeIdx]))
                    << kendl);
        (Void)(kout << "Size of BitMap (GIB): " << number(GIB(ptr_bit_set[kBitMapSizeIdx]))
                    << kendl);
        (Void)(kout << "Size of BitMap (TIB): " << number(TIB(ptr_bit_set[kBitMapSizeIdx]))
                    << kendl);
        (Void)(kout << "BitMap Address: " << hex_number((UIntPtr) ptr_bit_set) << kendl);
      }
    };
  }  // namespace Detail

  auto mm_is_bitmap(VoidPtr ptr) -> BOOL {
    Detail::IBitMapProxy bitmp;
    return bitmp.IsBitMap(ptr);
  }

  /***********************************************************************************/
  /// @brief Allocate a new page to be used by the OS.
  /// @param wr read/write bit.
  /// @param user user bit.
  /// @return a new bitmap allocated pointer.
  /***********************************************************************************/
  auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page, SizeT pad) -> VoidPtr {
    VoidPtr              ptr_new = nullptr;
    Detail::IBitMapProxy bitmp;

    if (is_page) return nullptr;

    ptr_new = bitmp.FindBitMap(kKernelBitMpStart, size, wr, user, pad);

    if (!ptr_new) {
      ke_panic(RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM, "Out of memory bitmap");
      return nullptr;
    }

    return ptr_new;
  }

  /***********************************************************************************/
  /// @brief Free Bitmap, and mark it as absent.
  /// @param ptr the pointer to free.
  /***********************************************************************************/
  auto mm_free_bitmap(VoidPtr ptr) -> Bool {
    if (!ptr) return No;

    Detail::IBitMapProxy bitmp;
    Bool                 ret = bitmp.FreeBitMap(ptr);

    return ret;
  }
}  // namespace HAL
}  // namespace Kernel
