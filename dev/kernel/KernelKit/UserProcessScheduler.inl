/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

	FILE: UserProcessScheduler.inl
	PURPOSE: Low level/Ring-3 process scheduler.

------------------------------------------- */

/// @brief USER_PROCESS inline definitions.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/// @date Tue Apr 22 22:01:07 CEST 2025

namespace Kernel
{
	/***********************************************************************************/
	/** @brief Free pointer from usage. */
	/***********************************************************************************/

	template <typename T>
	Boolean USER_PROCESS::Delete(ErrorOr<T*> ptr)
	{
		if (!ptr)
			return No;

		if (!this->HeapTree)
		{
			kout << "USER_PROCESS's heap is empty.\r";
			return No;
		}

		USER_HEAP_TREE* entry = this->HeapTree;

		while (entry != nullptr)
		{
			if (entry->MemoryEntry == ptr.Leak().Leak())
			{
				this->UsedMemory -= entry->MemoryEntrySize;

#ifdef __NE_AMD64__
				auto pd = hal_read_cr3();

				hal_write_cr3(this->VMRegister);

				auto ret = mm_delete_heap(entry->MemoryEntry);

				hal_write_cr3(pd);

				return ret == kErrorSuccess;
#else
				Bool ret = mm_delete_heap(ptr.Leak().Leak());

				return ret == kErrorSuccess;
#endif
			}

			entry = entry->MemoryNext;
		}

		kout << "USER_PROCESS: Trying to free a pointer which doesn't exist.\r";

		this->Crash();

		return No;
	}
} // namespace Kernel
