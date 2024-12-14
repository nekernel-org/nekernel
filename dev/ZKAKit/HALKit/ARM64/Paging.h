/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

	* THIS FILE CONTAINS CODE FOR ARMV8 PAGING.

------------------------------------------------------- */

#include <NewKit/Defines.h>

#ifndef kPageMax
#define kPageMax (0x200)
#endif //! kPageMax

#ifndef kPageAlign
#define kPageAlign (0x1000)
#endif //! kPageAlign

#ifndef kPageSize
#define kPageSize (0x1000)
#endif // !kPageSize

//! short format address range

#define c16KBPage 0b000
#define c8KBPage  0b001
#define c4KBPage  0b010
#define c2KBPage  0b011
#define c1KBPage  0b100
#define c512BPage 0b101
#define c256BPage 0b110
#define c128BPage 0b111

/// Long format address range

#define cPageMAll    \
	{                \
		0b000, 0b000 \
	}
#define cPageMToMax(M) \
	{                  \
		M, 0b000       \
	}
#define cPageMaxToM(M) \
	{                  \
		0b000, M       \
	}
#define cPageMToN(M, N) \
	{                   \
		M, N            \
	}

namespace Kernel::HAL
{
	struct PACKED PTE_4KB final
	{
		UInt64 Valid : 1;
		UInt64 Table : 1;
		UInt64 AttrIndex : 3;
		UInt64 NS : 1;
		UInt64 AP : 2;
		UInt64 SH : 2;
		UInt64 AF : 1;
		UInt64 NG : 1;
		UInt64 Reserved1 : 1;
		UInt64 Contiguous : 1;
		UInt64 Dirty : 1;
		UInt64 Reserved : 2;
		UInt64 PhysicalAddress : 36;
		UInt64 Reserved3 : 4;
		UInt64 PXN : 1;
		UInt64 XN : 1;
		UInt64 Reserved4 : 9;
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

	struct PDE_4KB final
	{
		PTE_4KB ALIGN(kPageAlign) fEntries[kPageMax];
	};

	auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page) -> VoidPtr;
	auto mm_free_bitmap(VoidPtr page_ptr) -> Bool;
} // namespace Kernel::HAL

namespace Kernel
{
	typedef HAL::PTE_4KB PTE;
	typedef HAL::PDE_4KB PDE;
} // namespace Kernel

EXTERN_C void hal_flush_tlb();
