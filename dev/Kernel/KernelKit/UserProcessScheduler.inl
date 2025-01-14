/* -------------------------------------------

	Copyright (C) 2024, t& Labs, all rights reserved.

	FILE: UserProcessScheduler.inl
	PURPOSE: Low level/Ring-3 Process scheduler.

------------------------------------------- */

namespace Kernel
{
	/***********************************************************************************/
	/** @brief Free pointer from usage. */
	/***********************************************************************************/

	template <typename T>
	Boolean UserProcess::Delete(ErrorOr<T*> ptr, const SizeT& sz)
	{
		if (!ptr ||
			sz == 0)
			return No;

		ProcessMemoryHeapList* entry = this->ProcessMemoryHeap;

		while (entry != nullptr)
		{
			if (entry->MemoryEntry == ptr.Leak().Leak())
			{
				this->UsedMemory -= entry->MemoryEntrySize;

#ifdef __ZKA_AMD64__
				auto pd = hal_read_cr3();
				hal_write_cr3(this->VMRegister);

				auto ret = mm_delete_heap(entry->MemoryEntry);

				hal_write_cr3(pd);

				return ret;
#else
				Bool ret = mm_delete_heap(ptr.Leak().Leak());
				return ret;
#endif
			}

			entry = entry->MemoryNext;
		}

		return No;
	}
} // namespace Kernel
