/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/ARM64/ApplicationProcessor.h>
#include <HALKit/ARM64/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/Timer.h>

#define GICD_BASE 0x08000000  // Distributor base address
#define GICC_BASE 0x08010000  // CPU interface base address

#define GICD_CTLR 0x000        // Distributor Control Register
#define GICD_ISENABLER 0x100   // Interrupt Set-Enable Registers
#define GICD_ICENABLER 0x180   // Interrupt Clear-Enable Registers
#define GICD_ISPENDR 0x200     // Interrupt Set-Pending Registers
#define GICD_ICPENDR 0x280     // Interrupt Clear-Pending Registers
#define GICD_IPRIORITYR 0x400  // Interrupt Priority Registers
#define GICD_ITARGETSR 0x800   // Interrupt Processor Targets Registers
#define GICD_ICFGR 0xC00       // Interrupt Configuration Registers

#define GICC_CTLR 0x000  // CPU Interface Control Register
#define GICC_PMR 0x004   // Interrupt Priority Mask Register
#define GICC_IAR 0x00C   // Interrupt Acknowledge Register
#define GICC_EOIR 0x010  // End of Interrupt Register

// ================================================================= //

namespace Kernel {
struct HAL_HARDWARE_THREAD final {
  HAL::StackFramePtr mFrame;
  ProcessID          mThreadID{0};
};

STATIC HAL_HARDWARE_THREAD kHWThread[kMaxAPInsideSched] = {{nullptr}};

namespace Detail {
  STATIC BOOL kGICEnabled = NO;

  STATIC void mp_hang_fn(void) {
    while (YES)
      ;

    dbg_break_point();
  }

  Void mp_setup_gic_el0(Void) {
    // enable distributor.
    ke_dma_write<UInt32>(GICD_BASE, GICD_CTLR, YES);

    UInt32 gicc_ctlr = ke_dma_read<UInt32>(GICC_BASE, GICC_CTLR);

    const auto kEnableSignalInt = YES;

    gicc_ctlr |= kEnableSignalInt;         // Enable signaling of interrupts
    gicc_ctlr |= (kEnableSignalInt << 1);  // Allow Group 1 interrupts in EL0

    ke_dma_write<UInt32>(GICC_BASE, GICC_CTLR, gicc_ctlr);

    // Set priority mask (accept all priorities)
    ke_dma_write<UInt32>(GICC_BASE, GICC_PMR, 0xFF);

    UInt32 icfgr = ke_dma_read<UInt32>(GICD_BASE, GICD_ICFGR + (0x20 / 0x10) * 4);

    icfgr |= (0x2 << ((32 % 16) * 2));  // Edge-triggered
    ke_dma_write<UInt32>(GICD_BASE, GICD_ICFGR + (0x20 / 0x10) * 4, icfgr);

    // Target interrupt 32 to CPU 1
    ke_dma_write<UInt32>(GICD_BASE, GICD_ITARGETSR + (0x20 / 0x04) * 4, 0x2 << ((32 % 4) * 8));

    // Set interrupt 32 priority to lowest (0xFF)
    ke_dma_write<UInt32>(GICD_BASE, GICD_IPRIORITYR + (0x20 / 0x04) * 4, 0xFF << ((32 % 4) * 8));

    // Enable interrupt 32 for AP.
    ke_dma_write<UInt32>(GICD_BASE, GICD_ISENABLER + 4, 0x01);
  }

  BOOL mp_handle_gic_interrupt_el0(Void) {
    // Read the interrupt ID
    UInt32 interrupt_id = ke_dma_read<UInt32>(GICC_BASE, GICC_IAR);

    // Check if it's a valid interrupt (not spurious)
    if ((interrupt_id & 0x3FF) < 1020) {
      auto interrupt = interrupt_id & 0x3FF;

      const UInt16 kInterruptScheduler = 0x20;

      (Void)(kout << "SMP: AP: " << hex_number(interrupt) << kendl);

      switch (interrupt) {
        case kInterruptScheduler: {
          ke_dma_write<UInt32>(GICC_BASE, GICC_EOIR, interrupt_id);
          UserProcessHelper::StartScheduling();
          break;
        }
        default: {
          ke_dma_write<UInt32>(GICC_BASE, GICC_EOIR, interrupt_id);
          break;
        }
      }

      return YES;
    }

    // spurious interrupt
    return NO;
  }
}  // namespace Detail

EXTERN_C HAL::StackFramePtr mp_get_current_context(ProcessID thrdid) {
  return kHWThread[thrdid].mFrame;
}

EXTERN_C Bool mp_register_process(HAL::StackFramePtr stack_frame, ProcessID thrdid) {
  MUST_PASS(Detail::kGICEnabled);

  if (!stack_frame) return NO;
  if (thrdid > kMaxAPInsideSched) return NO;

  const auto process_index = thrdid;

  kHWThread[process_index].mFrame    = stack_frame;
  kHWThread[process_index].mThreadID = thrdid;

  STATIC HardwareTimer timer{rtl_milliseconds(1000)};
  timer.Wait();

  HardwareThreadScheduler::The()[thrdid].Leak()->Busy(NO);

  return YES;
}

/// @internal
/// @brief Initialize the Global Interrupt Controller.
Void mp_init_cores(Void) noexcept {
  if (!Detail::kGICEnabled) {
    Detail::kGICEnabled = YES;
    Detail::mp_setup_gic_el0();
  }

  return Detail::kGICEnabled;
}
}  // namespace Kernel