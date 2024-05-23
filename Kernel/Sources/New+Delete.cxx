/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <KernelKit/KernelHeap.hpp>
#include <NewKit/New.hpp>

void* operator new[](size_t sz)
{
	if (sz == 0)
		++sz;

	return NewOS::ke_new_ke_heap(sz, true, false);
}

void* operator new(size_t sz)
{
	if (sz == 0)
		++sz;

	return NewOS::ke_new_ke_heap(sz, true, false);
}

void operator delete[](void* ptr)
{
	if (ptr == nullptr)
		return;

	NewOS::ke_delete_ke_heap(ptr);
}

void operator delete(void* ptr)
{
	if (ptr == nullptr)
		return;

	NewOS::ke_delete_ke_heap(ptr);
}

void operator delete(void* ptr, size_t sz)
{
	if (ptr == nullptr)
		return;

	NEWOS_UNUSED(sz);

	NewOS::ke_delete_ke_heap(ptr);
}
