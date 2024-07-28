/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

	* THIS FILE CONTAINS CODE FOR ARMV8 PAGING.

------------------------------------------------------- */

#include <NewKit/Defines.hpp>

#ifndef kPTEMax
#define kPTEMax (0x200)
#endif //! kPTEMax

#ifndef kPTEAlign
#define kPTEAlign (0x1000)
#endif //! kPTEAlign

#ifndef kPTESize
#define kPTESize (0x1000)
#endif // !kPTESize

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
	struct PACKED LongDescLevel3 final
	{
		Boolean Present : 1;
		Boolean Rw : 1;
		UInt16	Lpat : 9;
		UInt32	Address : 27;
		UInt32	Sbzp : 12;
		UInt32	UPat : 11;
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
		LongDescLevel3 ALIGN(kPTEAlign) Pte[kPTEMax];
	};

	VoidPtr hal_alloc_page(Boolean rw, Boolean user, SizeT size);
} // namespace Kernel::HAL

namespace Kernel
{
	typedef HAL::LongDescLevel3	 PTE;
	typedef HAL::PageDirectory64 PDE;
} // namespace Kernel

EXTERN_C void hal_flush_tlb();
