/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  FILE: UserProcessScheduler.inl
  PURPOSE: Low level/Ring-3 process scheduler.

------------------------------------------- */

/// @brief USER_PROCESS inline definitions.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/// @date Tue Apr 22 22:01:07 CEST 2025

#ifndef INC_PROCESS_SCHEDULER_H
#include <KernelKit/UserProcessScheduler.h>
#endif  // INC_PROCESS_SCHEDULER_H

namespace Kernel {
/***********************************************************************************/
/** @brief Free pointer from usage. */
/***********************************************************************************/

template <typename T>
BOOL USER_PROCESS::Delete(ErrorOr<T*> ptr) {
  if (!ptr) return No;

  if (!this->HeapTree) {
    kout << "USER_PROCESS's heap is empty.\r";
    return No;
  }

  PROCESS_HEAP_TREE<VoidPtr>* entry = this->HeapTree;

  while (entry != nullptr) {
    if (entry->Entry == ptr.Leak().Leak()) {
      this->UsedMemory -= entry->EntrySize;

#ifdef __NE_AMD64__
      auto pd = hal_read_cr3();

      hal_write_cr3(this->VMRegister);

      auto ret = mm_free_ptr(entry->Entry);

      hal_write_cr3(pd);

      return ret == kErrorSuccess;
#else
      Bool ret = mm_free_ptr(ptr.Leak().Leak());

      return ret == kErrorSuccess;
#endif
    }

    entry = entry->Next;
  }

  kout << "USER_PROCESS: Trying to free a pointer which doesn't exist.\r";

  this->Crash();

  return No;
}
}  // namespace Kernel
