/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Defines.h>
#include <Headers/Heap.h>

/// @brief Allocate from the user's heap.
/// @param sz size of object.
/// @param flags flags.
/// @return
CS_EXTERN_C PtrVoidType RtHeapAllocate(QWordType sz, DWordType flags)
{
	CS_MUST_PASS(kSharedApplication);
	CS_MUST_PASS(sz);
	CS_MUST_PASS(flags);

	return (PtrVoidType)kSharedApplication->Invoke(kSharedApplication,
												   kCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param ptr the pointer to free.
CS_EXTERN_C VoidType RtHeapFree(PtrVoidType ptr)
{
	CS_MUST_PASS(kSharedApplication);
	CS_MUST_PASS(ptr);

	CS_UNREFERENCED_PARAMETER(
		kSharedApplication->Invoke(kSharedApplication, kCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param ptr the pointer to find.
/// @return the size.
CS_EXTERN_C QWordType RtHeapGetSize(PtrVoidType ptr)
{
	CS_MUST_PASS(kSharedApplication);

	CS_MUST_PASS(ptr);
	return kSharedApplication->Invoke(kSharedApplication, kCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param ptr the pointer to check.
/// @return if it exists
CS_EXTERN_C BooleanType RtHeapPtrExists(PtrVoidType ptr)
{
	CS_MUST_PASS(kSharedApplication);

	CS_MUST_PASS(ptr);
	return kSharedApplication->Invoke(kSharedApplication, kCallCheckPtr, ptr);
}
