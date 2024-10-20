/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef _INC_KERNEL_HEAP_HXX_
#define _INC_KERNEL_HEAP_HXX_

// last-rev 30/01/24
// file: KernelHeap.hxx
// description: heap allocation for the Kernel.

#include <NewKit/Defines.hxx>

namespace Kernel
{
	/// @brief Declare pointer as free.
	/// @param heap_ptr the pointer.
	/// @return
	Int32 mm_delete_heap(VoidPtr heap_ptr);

	/// @brief Declare a new size for heap_ptr.
	/// @param heap_ptr the pointer.
	/// @return
	VoidPtr mm_realloc_heap(VoidPtr heap_ptr, SizeT new_sz);

	/// @brief Check if pointer is a valid Kernel pointer.
	/// @param heap_ptr the pointer
	/// @return if it exists.
	Boolean mm_is_valid_heap(VoidPtr heap_ptr);

	/// @brief Allocate chunk of memory.
	/// @param sz Size of pointer
	/// @param wr Read Write bit.
	/// @param user User enable bit.
	/// @return The newly allocated pointer.
	VoidPtr mm_new_heap(const SizeT sz, const Bool wr, const Bool user);

	/// @brief Protect the heap with a CRC value.
	/// @param heap_ptr pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean mm_protect_heap(VoidPtr heap_ptr);

	/// @brief Makes a Kernel page.
	/// @param heap_ptr the page pointer.
	/// @return status code
	Int32 mm_make_ke_page(VoidPtr heap_ptr);

	/// @brief Allocate C++ class.
	template <typename T, typename... Args>
	inline T* mm_new_class(Args&&... args)
	{
		T* cls = new T(move(args)...);
		return cls;
	}

	/// @brief Free C++ class.
	template <typename T>
	inline Void mm_delete_class(T* cls)
	{
		delete cls;
		cls = nullptr;
	}
} // namespace Kernel

#endif // !_INC_KERNEL_HEAP_HXX_
