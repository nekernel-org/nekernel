/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>

#ifdef __NE_AMD64__
#include <HALKit/AMD64/Paging.h>
#elif defined(__NE_ARM64__)
#include <HALKit/ARM64/Paging.h>
#endif

#include <NewKit/Defines.h>
#include <NewKit/KernelPanic.h>

#define kBitMapMagic   (0x10210U)
#define kBitMapPadSize (mib_cast(16))

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
			class IBitMapProxy final
			{
			public:
				explicit IBitMapProxy() = default;
				~IBitMapProxy()			= default;

				NE_COPY_DELETE(IBitMapProxy);

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
					if (!size)
						return nullptr;

					VoidPtr base = reinterpret_cast<VoidPtr>(((UIntPtr)base_ptr) + kPageSize);

					static SizeT biggest_block = 0UL;

					while (YES)
					{
						UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base);

						if (ptr_bit_set[kBitMapMagIdx] == kBitMapMagic &&
							ptr_bit_set[kBitMapSizeIdx] == size)
						{
							if (ptr_bit_set[kBitMapUsedIdx] == No)
							{
								ptr_bit_set[kBitMapSizeIdx] = size;
								ptr_bit_set[kBitMapUsedIdx] = Yes;

								this->GetBitMapStatus(ptr_bit_set);

								UInt32 flags = this->MakeMMFlags(wr, user);
								mm_map_page(ptr_bit_set, flags);

								if (biggest_block < size)
									biggest_block = size;

								return (VoidPtr)ptr_bit_set;
							}
						}
						else if (ptr_bit_set[kBitMapMagIdx] != kBitMapMagic)
						{
							ptr_bit_set[kBitMapMagIdx]	= kBitMapMagic;
							ptr_bit_set[kBitMapSizeIdx] = size;
							ptr_bit_set[kBitMapUsedIdx] = Yes;

							this->GetBitMapStatus(ptr_bit_set);

							UInt32 flags = this->MakeMMFlags(wr, user);
							mm_map_page(ptr_bit_set, flags);

							if (biggest_block < size)
								biggest_block = size;

							return (VoidPtr)ptr_bit_set;
						}

						base = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base) + ((ptr_bit_set[kBitMapMagIdx] != kBitMapMagic) ? (size) : ptr_bit_set[kBitMapSizeIdx]));
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
					kcout << "Address Of BitMap Header: " << hex_number((UIntPtr)ptr_bit_set) << endl;
				}
			};
		} // namespace Detail

		auto mm_is_bitmap(VoidPtr ptr) -> Bool
		{
			Detail::IBitMapProxy bitmp;
			return bitmp.IsBitMap(ptr);
		}

		/// @brief Allocate a new page to be used by the OS.
		/// @param wr read/write bit.
		/// @param user user bit.
		/// @return a new bitmap allocated pointer.
		auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page) -> VoidPtr
		{
			VoidPtr				 ptr_new = nullptr;
			Detail::IBitMapProxy bitmp;

			ptr_new = bitmp.FindBitMap(kKernelBitMpStart, size, wr, user);

			MUST_PASS(ptr_new);

			return (UIntPtr*)ptr_new;
		}

		/// @brief Free Bitmap, and mark it as absent.
		/// @param ptr the pointer to free.
		auto mm_free_bitmap(VoidPtr ptr) -> Bool
		{
			if (!ptr)
				return No;

			Detail::IBitMapProxy bitmp;
			Bool				 ret = bitmp.FreeBitMap(ptr);

			return ret;
		}
	} // namespace HAL
} // namespace Kernel
