/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	FILE: UserProcessScheduler.inl
	PURPOSE: Low level/Ring-3 Process scheduler.

------------------------------------------- */

namespace NeOS
{
	/***********************************************************************************/
	/** @brief Free pointer from usage. */
	/***********************************************************************************/

	template <typename T>
	Boolean UserProcess::Delete(ErrorOr<T*> ptr, const SizeT& sz)
	{
		if (!ptr)
			return No;

		if (!this->ProcessMemoryHeap)
		{
			kout << "Process Memory is empty.\r";
			return No;
		}

		ProcessMemoryHeapList* entry = this->ProcessMemoryHeap;

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

		kout << "Invalid Pointer: Trying to free a pointer which doesn't exist.\r";

		this->Crash();

		return No;
	}
} // namespace NeOS
