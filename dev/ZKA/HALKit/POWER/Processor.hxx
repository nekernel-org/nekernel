/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: POWER processor header.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>

#define NoOp()			 asm volatile("mr 0, 0")
#define kHalPPCAlignment __attribute__((aligned(4)))

namespace Kernel::HAL
{
	typedef UIntPtr Reg;

	/// @brief Stack frame (as retrieved from assembly.)
	struct PACKED StackFrame final
	{
		Reg IntNum, Exception;
		Reg A0, A2, BP, SP, A3, A4, A5, A6;
		Reg R8, R9, R10, R11, R12, R13, R14, R15;
		Reg Gs, Fs;
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
} // namespace Kernel::HAL

EXTERN_C Kernel::Void int_handle_math(Kernel::UIntPtr sp);
EXTERN_C Kernel::Void int_handle_pf(Kernel::UIntPtr sp);

/// @brief Set TLB.
Kernel::Bool hal_set_tlb(Kernel::UInt8 tlb, Kernel::UInt32 epn, Kernel::UInt64 rpn, Kernel::UInt8 perms, Kernel::UInt8 wimge, Kernel::UInt8 ts, Kernel::UInt8 esel, Kernel::UInt8 tsize, Kernel::UInt8 iprot);

/// @brief Write TLB.
Kernel::Void hal_write_tlb(Kernel::UInt32 mas0, Kernel::UInt32 mas1, Kernel::UInt32 mas2, Kernel::UInt32 mas3, Kernel::UInt32 mas7);

/// @brief Flush TLB.
EXTERN_C Kernel::Void hal_flush_tlb();
