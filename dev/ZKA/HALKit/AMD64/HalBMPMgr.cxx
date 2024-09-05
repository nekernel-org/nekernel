/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

#define cVMHMagic ((Kernel::UIntPtr)0x10210)

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
					while (base_ptr && size)
					{
						UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(base_ptr);

						if (ptr_bit_set[0] != cVMHMagic)
						{
							ptr_bit_set[0] = cVMHMagic;
							ptr_bit_set[1] = size;
							ptr_bit_set[2] = __BIGGEST_ALIGNMENT__;

							kcout << "ALLOC STATUS\r";
							kcout << "MAG: " << hex_number(ptr_bit_set[0]) << endl;
							kcout << "ADDRESS: " << hex_number((UIntPtr)ptr_bit_set) << endl;
							kcout << "SIZE: " << hex_number(ptr_bit_set[1]) << endl;
							kcout << "ALLOC STATUS\r";

							if (rw)
								mm_update_pte(base_ptr, eFlagsRw);

							if (user)
								mm_update_pte(base_ptr, eFlagsUser);

							return (VoidPtr)ptr_bit_set;
						}

						base_ptr = reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(base_ptr) + __BIGGEST_ALIGNMENT__ + ptr_bit_set[1]);
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

			return ((UIntPtr*)ptr_new);
		}

		auto mm_free_bitmap(VoidPtr page_ptr) -> Bool
		{
			if (!page_ptr)
				return false;

			UIntPtr* ptr_bit_set = reinterpret_cast<UIntPtr*>(page_ptr);

			if (!ptr_bit_set[0] ||
				ptr_bit_set[0] != cVMHMagic)
				return false;

			kcout << "FREE STATUS\r";
			kcout << "MAG: " << hex_number(ptr_bit_set[0]) << endl;
			kcout << "ADDRESSS: " << hex_number((UIntPtr)ptr_bit_set) << endl;
			kcout << "SIZE: " << hex_number(ptr_bit_set[1]) << endl;
			kcout << "FREE STATUS\r";

			ptr_bit_set[0] = 0UL;
			ptr_bit_set[1] = 0UL;
			ptr_bit_set[2] = __BIGGEST_ALIGNMENT__;

			return true;
		}
	} // namespace HAL
} // namespace Kernel
