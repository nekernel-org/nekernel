/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/Handover.h>

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
		kMMFlagsPresent = 1 << 0,
		kMMFlagsWr		= 1 << 1,
		kMMFlagsUser	= 1 << 2,
		kMMFlagsNX		= 1 << 3,
		kMMFlagsCount	= 3,
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

	inline Void rt_halt() noexcept
	{
		while (Yes)
		{
		}
	}

	template <typename DataKind>
	inline void hal_mmio_write(UIntPtr address, DataKind value)
	{
		*reinterpret_cast<volatile DataKind*>(address) = value;
	}

	template <typename DataKind>
	inline DataKind hal_mmio_read(UIntPtr address)
	{
		return *reinterpret_cast<volatile DataKind*>(address);
	}
} // namespace Kernel::HAL

inline Kernel::VoidPtr kKernelBitMpStart = nullptr;
inline Kernel::UIntPtr kKernelBitMpSize	 = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;

#include <HALKit/ARM64/Paging.h>
