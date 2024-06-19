/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

	* THIS FILE CONTAINS CODE FOR ARM64 PAGING.

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

EXTERN_C void hal_flush_tlb();

namespace NewOS::HAL
{
	struct PACKED PageTable64 final
	{
		bool		   Present : 1;
		bool		   Rw : 1;
		bool		   User : 1;
		bool		   Wt : 1;
		bool		   Cache : 1;
		bool		   Accessed : 1;
		NewOS::Int32   Reserved : 6;
		NewOS::UInt64 PhysicalAddress : 36;
		NewOS::Int32   Reserved1 : 15;
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

	VoidPtr hal_alloc_page(Boolean rw, Boolean user, SizeT size);
} // namespace NewOS::HAL

namespace NewOS
{
	typedef HAL::PageTable64	 PTE;
	typedef HAL::PageDirectory64 PDE;
} // namespace NewOS
