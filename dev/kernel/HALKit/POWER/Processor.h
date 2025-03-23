/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	Purpose: POWER processor header.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/Utils.h>

#define NoOp()			 asm volatile("mr 0, 0")
#define kHalPPCAlignment __attribute__((aligned(4)))

namespace NeOS::HAL
{
	typedef UIntPtr Reg;

	/// @brief Stack frame (as retrieved from assembly.)
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

	inline void rt_halt()
	{
		while (true)
		{
			NoOp(); // no oop.
		}
	}

	inline void rt_cli()
	{
		NoOp(); // no oop
	}
} // namespace NeOS::HAL

EXTERN_C NeOS::Void int_handle_math(NeOS::UIntPtr sp);
EXTERN_C NeOS::Void int_handle_pf(NeOS::UIntPtr sp);

/// @brief Set TLB.
NeOS::Bool hal_set_tlb(NeOS::UInt8 tlb, NeOS::UInt32 epn, NeOS::UInt64 rpn, NeOS::UInt8 perms, NeOS::UInt8 wimge, NeOS::UInt8 ts, NeOS::UInt8 esel, NeOS::UInt8 tsize, NeOS::UInt8 iprot);

/// @brief Write TLB.
NeOS::Void hal_write_tlb(NeOS::UInt32 mas0, NeOS::UInt32 mas1, NeOS::UInt32 mas2, NeOS::UInt32 mas3, NeOS::UInt32 mas7);

/// @brief Flush TLB.
EXTERN_C NeOS::Void hal_flush_tlb();
