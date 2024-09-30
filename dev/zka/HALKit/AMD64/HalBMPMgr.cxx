/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

#define cBitMpMagic ((Kernel::UIntPtr)0x10210)

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/Paging.hxx>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Paging.hxx>
#endif

#include <NewKit/Defines.hxx>
#include <NewKit/Stop.hxx>

#define cBitMapMagIdx  (0)
#define cBitMapSizeIdx (1)
#define cBitMapUsedIdx (2)

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

					if (!ptr_bit_set[cBitMapMagIdx] ||
						ptr_bit_set[cBitMapMagIdx] != cBitMpMagic)
						return No;

					return Yes;
				}

				auto FreeBitMap(VoidPtr page_ptr) -> Bool
				{
					if (this->IsBitMap(page_ptr) == No)
						return No;

					UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

					ptr_bit_set[cBitMapMagIdx]	= cBitMpMagic;
					ptr_bit_set[cBitMapUsedIdx] = No;

					this->GetBitMapStatus(ptr_bit_set);

					mm_map_page(ptr_bit_set, ~eFlagsPresent);
					mm_map_page(ptr_bit_set, ~eFlagsWr);
					mm_map_page(ptr_bit_set, ~eFlagsUser);

					return Yes;
				}

				UInt32 MakeFlags(Bool wr, Bool user)
				{

					UInt32 flags = eFlagsPresent;

					if (wr)
						flags |= eFlagsWr;

					if (user)
						flags |= eFlagsUser;

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

						if (ptr_bit_set[cBitMapMagIdx] == cBitMpMagic &&
							ptr_bit_set[cBitMapSizeIdx] <= size)
						{
							if (ptr_bit_set[cBitMapUsedIdx] == No)
							{
								ptr_bit_set[cBitMapSizeIdx] = size;
								ptr_bit_set[cBitMapUsedIdx] = Yes;

								this->GetBitMapStatus(ptr_bit_set);

								UInt32 flags = this->MakeFlags(wr, user);
								mm_map_page(ptr_bit_set, flags);

								return (VoidPtr)ptr_bit_set;
							}
						}
						else if (ptr_bit_set[cBitMapMagIdx] != cBitMpMagic)
						{
							UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

							ptr_bit_set[cBitMapMagIdx]	= cBitMpMagic;
							ptr_bit_set[cBitMapSizeIdx] = size;
							ptr_bit_set[cBitMapUsedIdx] = Yes;

							this->GetBitMapStatus(ptr_bit_set);

							UInt32 flags = this->MakeFlags(wr, user);
							mm_map_page(ptr_bit_set, flags);

							return (VoidPtr)ptr_bit_set;
						}

						base = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base_ptr) + (ptr_bit_set[0] != cBitMpMagic ? size : ptr_bit_set[1]));

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

					kcout << "Magic BitMap Number: " << hex_number(ptr_bit_set[cBitMapMagIdx]) << endl;
					kcout << "Allocated: " << (ptr_bit_set[cBitMapUsedIdx] ? "Yes" : "No") << endl;
					kcout << "Size of BitMap (B): " << number(ptr_bit_set[cBitMapSizeIdx]) << endl;
					kcout << "Size of BitMap (KIB): " << number(KIB(ptr_bit_set[cBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (MIB): " << number(MIB(ptr_bit_set[cBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (GIB): " << number(GIB(ptr_bit_set[cBitMapSizeIdx])) << endl;
					kcout << "Size of BitMap (TIB): " << number(TIB(ptr_bit_set[cBitMapSizeIdx])) << endl;
					kcout << "Address Of BitMap: " << hex_number((UIntPtr)ptr_bit_set) << endl;
				}
			};
		} // namespace Detail

		/// @brief Allocate a new page to be used by the OS.
		/// @param wr read/write bit.
		/// @param user user bit.
		/// @return
		auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size) -> VoidPtr
		{
			VoidPtr					 ptr_new = nullptr;
			Detail::IBitMapAllocator traits;

			ptr_new = traits.FindBitMap(kKernelBitMpStart, size, wr, user);

			if (!ptr_new)
			{
				ke_stop(RUNTIME_CHECK_PAGE);
			}

			if (wr)
				mm_map_page(ptr_new, eFlagsWr | eFlagsPresent);
			else if (user && wr)
				mm_map_page(ptr_new, eFlagsUser | eFlagsWr | eFlagsPresent);
			else if (user)
				mm_map_page(ptr_new, eFlagsUser | eFlagsPresent);
			else
				mm_map_page(ptr_new, eFlagsPresent);

			return (UIntPtr*)ptr_new;
		}

		auto mm_free_bitmap(VoidPtr page_ptr) -> Bool
		{
			if (!page_ptr)
				return No;

			Detail::IBitMapAllocator traits;
			Bool					 ret = traits.FreeBitMap(page_ptr);

			if (ret)
			{
				mm_map_page(page_ptr, ~eFlagsPresent);
			}

			return ret;
		}
	} // namespace HAL
} // namespace Kernel
