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
#include <NewKit/KernelCheck.hxx>

namespace Kernel
{
	namespace HAL
	{
		namespace Detail
		{
			struct IBitMapAllocator final
			{
				Bool FreeBitMap(VoidPtr page_ptr)
				{
					if (!page_ptr)
						return No;

					UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

					if (!ptr_bit_set[0] ||
						ptr_bit_set[0] != cBitMpMagic)
						return No;

					kcout << "BMPMgr: Freed Range!\r";
					kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
					kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
					kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
					kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
					kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
					kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
					kcout << "Address Of Header: " << hex_number((UIntPtr)ptr_bit_set) << endl;

					ptr_bit_set[0] = cBitMpMagic;
					ptr_bit_set[2] = No;

					mm_map_page(ptr_bit_set, ~eFlagsPresent);
					mm_map_page(ptr_bit_set, ~eFlagsRw);
					mm_map_page(ptr_bit_set, ~eFlagsUser);

					return Yes;
				}

				/// @brief Iterate over availables pages for a free one.
				/// @return The new address which was found.
				VoidPtr FindBitMap(VoidPtr base_ptr, SizeT size, Bool rw, Bool user) noexcept
				{
					auto base = reinterpret_cast<UIntPtr>(base_ptr);

					while (base_ptr && size)
					{
						UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

						if (ptr_bit_set[0] == cBitMpMagic)
						{
							if (ptr_bit_set[1] != 0 &&
								ptr_bit_set[1] <= size &&
								ptr_bit_set[2] == No)
							{
								ptr_bit_set[1] = size;
								ptr_bit_set[2] = Yes;

								kcout << "BMPMgr: Allocated Range!\r";
								kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
								kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
								kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
								kcout << "Address Of BMP: " << hex_number((UIntPtr)ptr_bit_set) << endl;

								return (VoidPtr)ptr_bit_set;
							}
						}
						else
						{
							UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

							ptr_bit_set[0] = cBitMpMagic;
							ptr_bit_set[1] = size;
							ptr_bit_set[2] = Yes;

							kcout << "BMPMgr: Allocated Range!\r";
							kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
							kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
							kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
							kcout << "Address Of BMP: " << hex_number((UIntPtr)ptr_bit_set) << endl;

							return (VoidPtr)ptr_bit_set;
						}

						base_ptr = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base_ptr) + (ptr_bit_set[0] != cBitMpMagic ? size : ptr_bit_set[1]));

						if ((UIntPtr)base_ptr < (base + kHandoverHeader->f_BitMapSize))
							return nullptr;
					}

					return nullptr;
				}
			};
		} // namespace Detail

		/// @brief Allocate a new page to be used by the OS.
		/// @param rw read/write bit.
		/// @param user user bit.
		/// @return
		auto mm_alloc_bitmap(Boolean rw, Boolean user, SizeT size) -> VoidPtr
		{
			VoidPtr					 ptr_new = nullptr;
			Detail::IBitMapAllocator traits;

			ptr_new = traits.FindBitMap(kKernelBitMpStart, size, rw, user);

			if (!ptr_new)
				return nullptr;

			if (rw)
				mm_map_page(ptr_new, eFlagsRw | eFlagsPresent);
			else if (user && rw)
				mm_map_page(ptr_new, eFlagsUser | eFlagsRw | eFlagsPresent);
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
