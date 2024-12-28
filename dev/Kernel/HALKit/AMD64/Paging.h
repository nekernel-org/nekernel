/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

	* THIS FILE CONTAINS CODE FOR X86_64 PAGING.

------------------------------------------------------- */

#include <NewKit/Defines.h>

#ifndef kPageMax
#define kPageMax (0x200)
#endif //! kPageMax

#ifndef kPageAlign
#define kPageAlign (0x08)
#endif //! kPageAlign

#ifndef kPageSize
#define kPageSize (0x100)
#endif // !kPageSize

#ifndef kAlign
#define kAlign __BIGGEST_ALIGNMENT__
#endif // !kAlign

EXTERN_C void hal_flush_tlb();
EXTERN_C void hal_invl_tlb(Kernel::VoidPtr addr);
EXTERN_C void hal_write_cr3(Kernel::VoidPtr cr3);
EXTERN_C void hal_write_cr0(Kernel::VoidPtr bit);

EXTERN_C Kernel::VoidPtr hal_read_cr0(); // @brief CPU control register.
EXTERN_C Kernel::VoidPtr hal_read_cr2(); // @brief Fault address.
EXTERN_C Kernel::VoidPtr hal_read_cr3(); // @brief Page table.

namespace Kernel::HAL
{
	/// @brief Final page entry (Not PML, PDPT)
	struct PACKED ZKA_PTE final
	{
		UInt64 Present : 1;
		UInt64 Wr : 1;
		UInt64 User : 1;
		UInt64 Wt : 1;
		UInt64 Cache : 1;
		UInt64 Accessed : 1;
		UInt64 Dirty : 1;
		UInt64 MemoryType : 1;
		UInt64 Global : 1;
		UInt64 Resvered1 : 3;
		UInt64 PhysicalAddress : 36;
		UInt64 Reserved2 : 10;
		UInt64 ProtectionKey : 5;
		UInt64 ExecDisable : 1;
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

	struct ZKA_PDE final
	{
		ZKA_PTE* ALIGN(kPageAlign) fEntries[kPageMax];
	};

	auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page) -> VoidPtr;
	auto mm_free_bitmap(VoidPtr page_ptr) -> Bool;
} // namespace Kernel::HAL

namespace Kernel
{
	typedef HAL::ZKA_PTE PTE;
	typedef HAL::ZKA_PDE PDE;
} // namespace Kernel
