/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/Paging.h>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Paging.h>
#endif

#include <NewKit/Defines.h>
#include <NewKit/Stop.h>

#define kBitMapMagic (0x10210U)

#define kBitMapMagIdx  (0U)
#define kBitMapSizeIdx (1U)
#define kBitMapUsedIdx (2U)

namespace Kernel
{
	namespace HAL
	{
		namespace Detail
		{
			/// \brief Proxy Interface to allocate a bitmap.
			class IBitMapAllocator final
			{
			public:
				explicit IBitMapAllocator() = default;
				~IBitMapAllocator()			= default;

				ZKA_COPY_DELETE(IBitMapAllocator);

				auto IsBitMap(VoidPtr page_ptr) -> Bool
				{
					if (!page_ptr)
						return No;

					UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

					if (!ptr_bit_set[kBitMapMagIdx] ||
						ptr_bit_set[kBitMapMagIdx] != kBitMapMagic)
						return No;

					return Yes;
				}

				auto FreeBitMap(VoidPtr page_ptr) -> Bool
				{
					if (this->IsBitMap(page_ptr) == No)
						return No;

					UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

					ptr_bit_set[kBitMapMagIdx]	= kBitMapMagic;
					ptr_bit_set[kBitMapUsedIdx] = No;

					this->GetBitMapStatus(ptr_bit_set);

					return Yes;
				}

				UInt32 MakeMMFlags(Bool wr, Bool user)
				{
					UInt32 flags = kMMFlagsPresent;

					if (wr)
						flags |= kMMFlagsWr;

					if (user)
						flags |= kMMFlagsUser;

					return flags;
				}

				/// @brief Iterate over availables pages for a free one.
				/// @return The new address which was found.
				auto FindBitMap(VoidPtr base_ptr, SizeT size, Bool wr, Bool user) -> VoidPtr
				{
					VoidPtr base = reinterpret_cast<VoidPtr>(((UIntPtr)base_ptr) + kPageSize);

					while (base && size)
					{
						UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base);

						if (ptr_bit_set[kBitMapMagIdx] == kBitMapMagic &&
							ptr_bit_set[kBitMapSizeIdx] <= size)
						{
							if (ptr_bit_set[kBitMapUsedIdx] == No)
							{
								ptr_bit_set[kBitMapSizeIdx] = size;
								ptr_bit_set[kBitMapUsedIdx] = Yes;

								this->GetBitMapStatus(ptr_bit_set);

								UInt32 flags = this->MakeMMFlags(wr, user);
								mm_map_page(ptr_bit_set, flags);

								return (VoidPtr)ptr_bit_set;
							}
						}
						else if (ptr_bit_set[kBitMapMagIdx] != kBitMapMagic)
						{
							UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

							ptr_bit_set[kBitMapMagIdx]	= kBitMapMagic;
							ptr_bit_set[kBitMapSizeIdx] = size;
							ptr_bit_set[kBitMapUsedIdx] = Yes;

							this->GetBitMapStatus(ptr_bit_set);

							UInt32 flags = this->MakeMMFlags(wr, user);
							mm_map_page(ptr_bit_set, flags);

							return (VoidPtr)ptr_bit_set;
						}

						base = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base_ptr) + (ptr_bit_set[0] != kBitMapMagic ? size : ptr_bit_set[1]));

						if ((UIntPtr)base_ptr < (reinterpret_cast<UIntPtr>(base) + kHandoverHeader->f_BitMapSize))
							return nullptr;
					}

					return nullptr;
				}

				/// @brief Print Bitmap status
				auto GetBitMapStatus(UIntPtr* ptr_bit_set) -> Void
				{
					if (!this->IsBitMap(ptr_bit_set))
					{
						kcout << "Not a BitMap: " << hex_number((UIntPtr)ptr_bit_set) << endl;
						return;
					}

					kcout << "Magic Number: " << hex_number(ptr_bit_set[kBitMapMagIdx]) << endl;
					kcout << "Is Allocated: " << (ptr_bit_set[kBitMapUsedIdx] ? "Yes" : "No") << endl;
					kcout << "Size of BitMap (B): " << number(ptr_bit_set[kBitMapSizeIdx]) << endl;
					kcout << "Size of BitMap (KIB): " << number(KIB(ptr_bit_set[kBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (MIB): " << number(MIB(ptr_bit_set[kBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (GIB): " << number(GIB(ptr_bit_set[kBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (TIB): " << number(TIB(ptr_bit_set[kBitMapSizeIdx])) << endl;
					kcout << "Address Of BitMap: " << hex_number((UIntPtr)ptr_bit_set) << endl;
				}
			};
		} // namespace Detail

		/// @brief Allocate a new page to be used by the OS.
		/// @param wr read/write bit.
		/// @param user user bit.
		/// @return a new bitmap allocated pointer.
		auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page) -> VoidPtr
		{
			VoidPtr					 ptr_new = nullptr;
			Detail::IBitMapAllocator traits;

			ptr_new = traits.FindBitMap(kKernelBitMpStart, size, wr, user);

			return (UIntPtr*)ptr_new;
		}

		/// @brief Free Bitmap, and mark it a absent in page terms.
		auto mm_free_bitmap(VoidPtr page_ptr) -> Bool
		{
			if (!page_ptr)
				return No;

			Detail::IBitMapAllocator traits;
			Bool					 ret = traits.FreeBitMap(page_ptr);

			return ret;
		}
	} // namespace HAL
} // namespace Kernel
