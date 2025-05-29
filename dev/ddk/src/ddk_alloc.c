/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK allocator.

------------------------------------------- */

#include <DDKKit/ddk.h>

/**
  \brief Allocates a new heap on the Kernel's side.
  \param sz the size of the heap block.
  \return the newly allocated pointer.
*/
DDK_EXTERN void* kalloc(size_t sz) {
  if (!sz) ++sz;

  void* ptr = ke_call("mm_alloc_ptr", 1, &sz, sizeof(size_t));

  return ptr;
}

/**
  \brief Frees a pointer from the heap.
  \param ptr the pointer to free.
*/
DDK_EXTERN void kfree(void* ptr) {
  if (!ptr) return;

  ke_call("mm_free_ptr", 1, ptr, 0);
}
