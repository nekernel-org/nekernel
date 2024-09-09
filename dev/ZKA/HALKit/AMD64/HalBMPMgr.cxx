/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

#define cBitMpMagic ((Kernel::UIntPtr)0x10210)

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hxx>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/HalPageAlloc.hxx>
#endif

#include <NewKit/Defines.hxx>
#include <NewKit/KernelCheck.hxx>

namespace Kernel
{
	namespace HAL
	{
		namespace Detail
		{
			struct AllocatorTraits final
			{
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

								kcout << "BMPMgr: Allocated pointer!\r";
								kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
								kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
								kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
								kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
								kcout << "Address Of BMP: " << hex_number((UIntPtr)ptr_bit_set) << endl;

								if (rw)
									mm_map_page(base_ptr, eFlagsRw | eFlagsPresent);

								if (user)
									mm_map_page(base_ptr, eFlagsUser | eFlagsPresent);

								return (VoidPtr)ptr_bit_set;
							}
						}
						else
						{
							UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

							ptr_bit_set[0] = cBitMpMagic;
							ptr_bit_set[1] = size;
							ptr_bit_set[2] = Yes;

							kcout << "BMPMgr: Allocated pointer!\r";
							kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
							kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
							kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
							kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
							kcout << "Address Of BMP: " << hex_number((UIntPtr)ptr_bit_set) << endl;

							if (rw)
								mm_map_page(base_ptr, eFlagsRw | eFlagsPresent);

							if (user)
								mm_map_page(base_ptr, eFlagsUser | eFlagsPresent);

							return (VoidPtr)ptr_bit_set;
						}

						base_ptr = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base_ptr) + (ptr_bit_set[0] != cBitMpMagic ? size : ptr_bit_set[1]));

						if (reinterpret_cast<UIntPtr>(base_ptr) >= (kHandoverHeader->f_BitMapSize + base))
							break;
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
			VoidPtr					ptr_new = nullptr;
			Detail::AllocatorTraits traits;

			ptr_new = traits.FindBitMap(kKernelVirtualStart, size, rw, user);

			if (!ptr_new)
			{
				ke_stop(RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM);
			}

			return ((UIntPtr*)ptr_new);
		}

		auto mm_free_bitmap(VoidPtr page_ptr) -> Bool
		{
			if (!page_ptr)
				return false;

			UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

			if (!ptr_bit_set[0] ||
				ptr_bit_set[0] != cBitMpMagic)
				return false;

			kcout << "BMPMgr: Freed pointer!\r";
			kcout << "Magic Number: " << hex_number(ptr_bit_set[0]) << endl;
			kcout << "Size of pointer (B): " << number(ptr_bit_set[1]) << endl;
			kcout << "Size of pointer (KIB): " << number(KIB(ptr_bit_set[1])) << endl;
			kcout << "Size of pointer (MIB): " << number(MIB(ptr_bit_set[1])) << endl;
			kcout << "Size of pointer (GIB): " << number(GIB(ptr_bit_set[1])) << endl;
			kcout << "Size of pointer (TIB): " << number(TIB(ptr_bit_set[1])) << endl;
			kcout << "Address Of Header: " << hex_number((UIntPtr)ptr_bit_set) << endl;

			ptr_bit_set[0] = cBitMpMagic;
			ptr_bit_set[2] = No;

			mm_map_page(page_ptr, ~eFlagsPresent | ~eFlagsUser);

			return true;
		}
	} // namespace HAL
} // namespace Kernel
