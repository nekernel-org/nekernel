/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <NewKit/String.hpp>

/// BUGS = 1

/// @brief System call interrupt (like DOS and NT)
#define kKernelSyscallInterrupt (0x21)

EXTERN_C {
  HCore::Void rt_handle_interrupts(HCore::HAL::StackFramePtr stack) {
    HCore::kcout << "HCoreKrnl: Interrupting Hart...\r\n";

    switch (stack->IntNum) {
      case kKernelSyscallInterrupt: {
        HCore::kcout << "HCoreKrnl: System call raised, checking.."
                     << HCore::end_line();
        rt_syscall_handle(stack);
        break;
      }

      default:
        break;
    }

    if ((stack->IntNum - 32) >= 12) {
      HCore::kcout << "HCoreKrnl: EOI Master PIC...\r\n";
      HCore::HAL::Out8(0xA0, 0x20);
    }

    HCore::kcout << "HCoreKrnl: EOI Slave PIC...\r\n";
    HCore::HAL::Out8(0x20, 0x20);
  }
}