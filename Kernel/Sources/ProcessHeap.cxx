/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/PageManager.hxx>

#define cHeapHeaderPaddingSz (16U)

/// @file ProcessHeap.cxx
/// @brief User Heap Manager, Process heap allocator.
/// @note if you want to look at the kernel allocator, please look for
/// KernelHeap.cxx
/// BUGS: 0

namespace Kernel
{
	/**
	 * @brief Process Heap Header
	 * @note Allocated per process, it denotes the user's heap.
	 */
	struct PROCESS_HEAP_HEADER final
	{
		UInt32	fPageMagic;
		Int32	fPageFlags;
		Boolean fPageFree;
		UIntPtr fPageVirtStart;
		SizeT   fPageVirtSize;
		UInt8	fPagePad[cHeapHeaderPaddingSz];
	};

	/// @brief PROCESS_HEAP_HEADER as pointer type.
	typedef PROCESS_HEAP_HEADER* PROCESS_HEAP_HEADER_PTR;

	/**
	 * @brief Process heap class, takes care of allocating the process pools.
	 * @note This rely on Virtual Memory! Consider adding good vmem support when
	 * @note porting to a new arch.
	 */
	class ProcessHeapHelper final
	{
		ProcessHeapHelper() = delete;

	public:
		~ProcessHeapHelper() = default;

	public:
		STATIC SizeT& Count() noexcept
		{
			return s_NumPools;
		}

		STATIC Ref<Pmm>& Leak() noexcept
		{
			return s_Pmm;
		}

		STATIC Boolean& IsEnabled() noexcept
		{
			return s_PoolsAreEnabled;
		}

		STATIC MutableArray<Ref<PTEWrapper>>& The() noexcept
		{
			return s_Pool;
		}

	private:
		STATIC Size s_NumPools;
		STATIC Ref<Pmm> s_Pmm;

	private:
		STATIC Boolean s_PoolsAreEnabled;
		STATIC MutableArray<Ref<PTEWrapper>> s_Pool;
	};

	//! declare fields

	SizeT						  ProcessHeapHelper::s_NumPools = 0UL;
	Ref<Pmm>					  ProcessHeapHelper::s_Pmm;
	Boolean						  ProcessHeapHelper::s_PoolsAreEnabled = true;
	MutableArray<Ref<PTEWrapper>> ProcessHeapHelper::s_Pool;

	STATIC VoidPtr sched_find_unused_heap(Int32 flags, SizeT len);
	STATIC Void	   sched_free_heap_internal(VoidPtr vaddr);
	STATIC VoidPtr sched_make_heap_internal(VoidPtr vaddr, Int32 flags, SizeT len);
	STATIC Boolean sched_check_and_free_heap(const SizeT& index, VoidPtr ptr);

	/// @brief Find an unused heap header to allocate on.
	/// @param flags the flags to use.
	/// @return VoidPtr the heap pointer.
	STATIC VoidPtr sched_find_unused_heap(Int32 flags, SizeT len)
	{
		SizeT index = 0UL;

		while (true)
		{
			/* ************************************ */
			/* allocate if it doesnt exist. */
			/* ************************************ */
			if (!ProcessHeapHelper::The()[index])
			{
				ProcessHeapHelper::The().Add(Kernel::Ref<Kernel::PTEWrapper>());
			}

			if (ProcessHeapHelper::The()[index] &&
				!ProcessHeapHelper::The()[index].Leak().Leak().Present())
			{
				ProcessHeapHelper::Leak().Leak().TogglePresent(
					ProcessHeapHelper::The()[index].Leak().Leak(), true);
					
				ProcessHeapHelper::Leak().Leak().ToggleUser(
					ProcessHeapHelper::The()[index].Leak().Leak(), true);

				kcout << "[sched_find_unused_heap] Done, trying to make a pool now...\r";

				return sched_make_heap_internal(
					(VoidPtr)ProcessHeapHelper::The()[index].Leak().Leak().VirtualAddress(),
					flags, len);
			}

			++index;
		}

		return nullptr;
	}

	/// @brief Makes a new heap for the process to use.
	/// @param virtual_address the virtual address of the process.
	/// @param flags the flags.
	/// @return
	STATIC VoidPtr sched_make_heap_internal(VoidPtr virtual_address, Int32 flags, SizeT len_in_gb)
	{
		if (virtual_address)
		{
			PROCESS_HEAP_HEADER* process_heap_hdr = reinterpret_cast<PROCESS_HEAP_HEADER*>(virtual_address);

			if (!process_heap_hdr->fPageFree)
			{
				kcout
					<< "[sched_make_heap_internal] process_heap_hdr->fPageFree, HeapPtr already exists\n";
				return nullptr;
			}

			process_heap_hdr->fPageFlags = flags;
			process_heap_hdr->fPageMagic = kProcessHeapMag;
			process_heap_hdr->fPageFree	= false;
			process_heap_hdr->fPageVirtStart = (UIntPtr)virtual_address + sizeof(PROCESS_HEAP_HEADER);
			process_heap_hdr->fPageVirtSize = len_in_gb;

			kcout << "[sched_make_heap_internal] New allocation has been done, returning new chunk.\n";

			return reinterpret_cast<VoidPtr>(
				(reinterpret_cast<UIntPtr>(virtual_address) + sizeof(PROCESS_HEAP_HEADER)));
		}

		kcout << "[sched_make_heap_internal] Address is invalid";
		return nullptr;
	}

	/// @brief Internally makrs the heap as free.
	/// This is done by setting the fPageFree bit to true
	/// @param virtual_address
	/// @return
	STATIC Void sched_free_heap_internal(VoidPtr virtual_address)
	{
		PROCESS_HEAP_HEADER* process_heap_hdr = reinterpret_cast<PROCESS_HEAP_HEADER*>(
			reinterpret_cast<UIntPtr>(virtual_address) - sizeof(PROCESS_HEAP_HEADER));

		if (process_heap_hdr->fPageMagic == kProcessHeapMag)
		{
			if (!process_heap_hdr->fPageFree)
			{
				ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
				return;
			}

			process_heap_hdr->fPageFree	= true;
			process_heap_hdr->fPageFlags = 0;

			kcout << "[sched_free_heap_internal] Successfully marked header as free!\r";
		}
	}

	/**
	 * @brief Check for the ptr and frees it.
	 *
	 * @param index Where to look at.
	 * @param ptr The ptr to check.
	 * @return Boolean true if successful.
	 */
	STATIC Boolean sched_check_and_free_heap(const SizeT& index, VoidPtr ptr)
	{
		if (ProcessHeapHelper::The()[index])
		{
			// ErrorOr<>::operator Boolean
			/// if (address matches)
			///     -> Free heap.
			if (ProcessHeapHelper::The()[index].Leak().Leak().VirtualAddress() ==
				(UIntPtr)ptr)
			{
				ProcessHeapHelper::Leak().Leak().FreePage(
					ProcessHeapHelper::The()[index].Leak().Leak());

				--ProcessHeapHelper::Count();

				sched_free_heap_internal(ptr);
				ptr = nullptr;

				return true;
			}
		}

		return false;
	}

	/// @brief Creates a new pool pointer.
	/// @param flags the flags attached to it.
	/// @return a pool pointer with selected permissions.
	VoidPtr sched_new_heap(Int32 flags, SizeT page_size)
	{
		if (!ProcessHeapHelper::IsEnabled())
			return nullptr;

		if (VoidPtr ret = sched_find_unused_heap(flags, page_size))
			return ret;

		// this wasn't set to true
		auto ref_page = ProcessHeapHelper::Leak().Leak().RequestPage(
			((flags & kProcessHeapUser)), (flags & kProcessHeapRw));

		if (ref_page)
		{
			///! reserve page.
			ProcessHeapHelper::The()[ProcessHeapHelper::Count()].Leak() = ref_page;
			auto& ref												= ProcessHeapHelper::Count();

			++ref; // increment the number of addresses we have now.

			// finally make the pool address.
			return sched_make_heap_internal(
				reinterpret_cast<VoidPtr>(ref_page.Leak().VirtualAddress()), flags, page_size);
		}

		return nullptr;
	}

	/// @brief free a pool pointer.
	/// @param ptr The pool pointer to free.
	/// @return status code
	Int32 sched_free_heap(VoidPtr ptr)
	{
		if (!ProcessHeapHelper::IsEnabled())
			return -1;

		if (ptr)
		{
			SizeT base = ProcessHeapHelper::Count();

			if (sched_check_and_free_heap(base, ptr))
				return 0;

			for (SizeT index = 0; index < ProcessHeapHelper::The().Count(); ++index)
			{
				if (sched_check_and_free_heap(index, ptr))
					return 0;

				--base;
			}
		}

		return -1;
	}
} // namespace Kernel
