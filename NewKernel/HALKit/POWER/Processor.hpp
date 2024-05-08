/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: POWER processor header.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#define kHalPPCAlignment __attribute__((aligned(4)))

namespace NewOS::HAL
{
	typedef UIntPtr Reg;

	struct kHalPPCAlignment StackFrame
	{
		Reg R0;
		Reg R1;
		Reg R2;
		Reg R3;
		Reg R4;
		Reg R5;
		Reg R6;
		Reg R7;
		Reg R8;
		Reg PC;
		Reg SP;
	};

	typedef StackFrame* StackFramePtr;

	inline void rt_halt()
	{
		while (1)
		{
			asm volatile("mr 0, 0"); // no oop.
		}
	}

	inline void rt_cli()
	{
		asm volatile("mr 0, 0"); // no oop
	}
} // namespace NewOS::HAL

EXTERN_C void int_handle_math(NewOS::UIntPtr sp);
EXTERN_C void int_handle_pf(NewOS::UIntPtr sp);

/// @brief Flush system TLB.
EXTERN_C void hal_flush_tlb();
