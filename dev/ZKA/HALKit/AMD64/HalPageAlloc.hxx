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
EXTERN_C void hal_write_cr3(Kernel::UIntPtr phys_addr);
EXTERN_C void hal_write_cr0(Kernel::UIntPtr bit);

EXTERN_C Kernel::VoidPtr hal_read_cr0(); // @brief CPU control register.
EXTERN_C Kernel::VoidPtr hal_read_cr2(); // @brief Fault address.
EXTERN_C Kernel::VoidPtr hal_read_cr3(); // @brief Page table.

namespace Kernel::HAL
{
	struct PACKED ZKA_PTE final
	{
		UInt8  Present : 1;
		UInt8  Rw : 1;
		UInt8  User : 1;
		UInt8  Wt : 1;
		UInt8  Cache : 1;
		UInt8  Accessed : 1;
		UInt8  Dirty : 1;
		UInt8  PageSize : 1;
		UInt8  Global : 1;
		UInt8  Available : 3;
		UInt64 PhysicalAddress : 39;
		UInt8  Reserved : 6;
		UInt8  ProtectionKey : 1;
		UInt8  ExecDisable : 1;
		UInt8  ReservedEx : 3;
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
		ZKA_PTE ALIGN(kPTEAlign) Pte[kPTEMax];
	};

	auto hal_alloc_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr;
	auto hal_free_page(VoidPtr page_ptr) -> Bool;
} // namespace Kernel::HAL

namespace Kernel
{
	typedef HAL::ZKA_PTE PTE;
	typedef HAL::ZKA_PDE PDE;
} // namespace Kernel
