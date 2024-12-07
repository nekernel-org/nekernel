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

		UserProcessHeapList* entry = this->ProcessMemoryHeap;

		while (entry != nullptr)
		{
			if (entry->MemoryEntry == ptr.Leak().Leak())
			{
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
