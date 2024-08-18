/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/Heap.hxx>
#include <NewKit/New.hxx>

void* operator new[](size_t sz)
{
	if (sz == 0)
		++sz;

	return Kernel::mm_new_ke_heap(sz, true, false);
}

void* operator new(size_t sz)
{
	if (sz == 0)
		++sz;

	return Kernel::mm_new_ke_heap(sz, true, false);
}

void operator delete[](void* ptr)
{
	if (ptr == nullptr)
		return;

	Kernel::mm_delete_ke_heap(ptr);
}

void operator delete(void* ptr)
{
	if (ptr == nullptr)
		return;

	Kernel::mm_delete_ke_heap(ptr);
}

void operator delete(void* ptr, size_t sz)
{
	if (ptr == nullptr)
		return;

	NEWOS_UNUSED(sz);

	Kernel::mm_delete_ke_heap(ptr);
}
