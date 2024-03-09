/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//! @file InterruptHandler.cxx
//! @brief x64 interrupt handler.

#include <ArchKit/ArchKit.hpp>

#define kInterruptId    0x21

/// @brief Runtime interrupt handler
/// @param sf The stack frame pushed by the isr.
/// @return the stackframe pointer.
EXTERN_C ATTRIBUTE(naked) HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr sf)
{
    HCore::HAL::StackFramePtr stackPtr = reinterpret_cast<HCore::HAL::StackFramePtr>(sf);
    MUST_PASS(stackPtr);

    switch (stackPtr->IntNum)
    {
    case kInterruptId:
    {
        /* TODO: HcOpenDevice and such syscalls. */
        break;
    }
    default:
        break;
    }

    return sf;
}