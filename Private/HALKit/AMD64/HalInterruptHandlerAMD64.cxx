/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//! @file InterruptHandler.cxx
//! @brief x64 interrupt handler.

#include <ArchKit/ArchKit.hpp>

/// @brief Interrupt handler 21h, 10h 
/// @param rsp stack pointer.

#define kInterruptIdAlt 0x10
#define kInterruptId    0x21

EXTERN_C ATTRIBUTE(naked) HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr rsp)
{
    HCore::HAL::StackFramePtr stackPtr = reinterpret_cast<HCore::HAL::StackFramePtr>(rsp);
    HCORE_UNUSED(stackPtr);

    if (stackPtr->IntNum == kInterruptId ||
        stackPtr->IntNum == kInterruptIdAlt) {
        /// Do system call TODO
        
    }

    return rsp;
}