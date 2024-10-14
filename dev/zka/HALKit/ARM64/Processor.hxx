/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <FirmwareKit/Handover.hxx>

#define kCPUBackendName "ARMv8"

namespace Kernel::HAL
{
	struct PACKED Register64 final
	{
		UShort	Limit;
		UIntPtr Base;
	};

	/// @brief Memory Manager mapping flags.
	enum
	{
		eFlagsPresent = 1 << 0,
		eFlagsWr	  = 1 << 1,
		eFlagsUser	  = 1 << 2,
		eFlagsNX	  = 1 << 3,
		eFlagsCount	  = 3,
	};

	/// @brief Set a PTE from pd_base.
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manip.
	EXTERN_C Int32 mm_map_page(VoidPtr virt_addr, UInt32 flags);

	typedef UIntPtr	   Reg;
	typedef Register64 Register;

	/// @note let's keep the same name as AMD64 HAL.
	struct PACKED StackFrame final
	{
		Reg R8{0};
		Reg R9{0};
		Reg R10{0};
		Reg R11{0};
		Reg R12{0};
		Reg R13{0};
		Reg R14{0};
		Reg R15{0};
		Reg SP{0};
		Reg BP{0};
	};

	typedef StackFrame* StackFramePtr;

	inline Void rt_halt()
	{
		while (Yes)
		{

		}
	}

} // namespace Kernel::HAL

inline Kernel::VoidPtr kKernelBitMpStart = nullptr;
inline Kernel::UIntPtr kKernelBitMpSize	 = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;

#include <HALKit/ARM64/Paging.hxx>
