/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

	* THIS FILE CONTAINS CODE FOR X86_64 PAGING.

------------------------------------------------------- */

#include <NewKit/Defines.hxx>

#ifndef kPTEMax
#define kPTEMax (0x200)
#endif //! kPTEMax

#ifndef kPTEAlign
#define kPTEAlign (0x1000)
#endif //! kPTEAlign

#ifndef kPTESize
#define kPTESize (0x1000)
#endif // !kPTESize

#ifndef kAlign
#define kAlign __BIGGEST_ALIGNMENT__
#endif // !kAlign

EXTERN_C void hal_flush_tlb();
EXTERN_C void hal_write_cr3(Kernel::UIntPtr pde);
EXTERN_C void hal_write_cr0(Kernel::UIntPtr bit);

EXTERN_C Kernel::UIntPtr hal_read_cr0(); // @brief CPU control register.
EXTERN_C Kernel::UIntPtr hal_read_cr2(); // @brief Fault address.
EXTERN_C Kernel::UIntPtr hal_read_cr3(); // @brief Page table.

namespace Kernel::HAL
{
	struct PACKED PageTable64 final
	{
		bool		   Present : 1;
		bool		   Rw : 1;
		bool		   User : 1;
		bool		   Wt : 1;
		bool		   Cache : 1;
		bool		   Accessed : 1;
		Kernel::Int32  Reserved : 6;
		Kernel::UInt64 PhysicalAddress : 36;
		Kernel::Int32  Reserved1 : 15;
		bool		   ExecDisable : 1;
	};

	namespace Detail
	{
		enum class ControlRegisterBits
		{
			ProtectedModeEnable = 0,
			MonitorCoProcessor	= 1,
			Emulation			= 2,
			TaskSwitched		= 3,
			ExtensionType		= 4,
			NumericError		= 5,
			WriteProtect		= 16,
			AlignementMask		= 18,
			NotWriteThrough		= 29,
			CacheDisable		= 30,
			PageEnable			= 31,
		};

		inline UInt8 control_register_cast(ControlRegisterBits reg)
		{
			return static_cast<UInt8>(reg);
		}
	} // namespace Detail

	struct PageDirectory64 final
	{
		PageTable64 ALIGN(kPTEAlign) Pte[kPTEMax];
	};

	auto hal_alloc_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr;
	auto hal_free_page(VoidPtr page_ptr) -> Bool;
} // namespace Kernel::HAL

namespace Kernel
{
	typedef HAL::PageTable64	 PTE;
	typedef HAL::PageDirectory64 PDE;
} // namespace Kernel
