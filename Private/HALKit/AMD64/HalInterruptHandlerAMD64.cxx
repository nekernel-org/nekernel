/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//! @file InterruptHandler.cxx
//! @brief x64 interrupt handler.

#include <ArchKit/ArchKit.hpp>

/// @brief AMD64 Interrupt handler redirects interrupts to their handlers. 
EXTERN_C ATTRIBUTE(naked) HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr rsp)
{
    HCore::HAL::StackFramePtr stackPtr = reinterpret_cast<HCore::HAL::StackFramePtr>(rsp);
    HCORE_UNUSED(stackPtr);

    return rsp;
}