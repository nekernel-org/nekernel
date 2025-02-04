/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/ARM64/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <HALKit/ARM64/ApplicationProcessor.h>
#include <KernelKit/UserProcessScheduler.h>

#define GICD_BASE 0x08000000 // Distributor base address
#define GICC_BASE 0x08010000 // CPU interface base address

#define GICD_CTLR		0x000 // Distributor Control Register
#define GICD_ISENABLER	0x100 // Interrupt Set-Enable Registers
#define GICD_ICENABLER	0x180 // Interrupt Clear-Enable Registers
#define GICD_ISPENDR	0x200 // Interrupt Set-Pending Registers
#define GICD_ICPENDR	0x280 // Interrupt Clear-Pending Registers
#define GICD_IPRIORITYR 0x400 // Interrupt Priority Registers
#define GICD_ITARGETSR	0x800 // Interrupt Processor Targets Registers
#define GICD_ICFGR		0xC00 // Interrupt Configuration Registers

#define GICC_CTLR 0x000 // CPU Interface Control Register
#define GICC_PMR  0x004 // Interrupt Priority Mask Register
#define GICC_IAR  0x00C // Interrupt Acknowledge Register
#define GICC_EOIR 0x010 // End of Interrupt Register

// ================================================================= //

namespace Kernel
{
	struct PROCESS_CONTROL_BLOCK final
	{
		HAL::StackFramePtr mFrame;
	};

	STATIC PROCESS_CONTROL_BLOCK kProcessBlocks[kSchedProcessLimitPerTeam] = {0};

	namespace Detail
	{
		STATIC BOOL kGICEnabled = NO;

		STATIC void mp_hang_fn(void)
		{
			while (YES)
				;
		}

		Void mp_setup_gic_el0(Void)
		{
			// enable distributor.
			HAL::hal_mmio_write(GICD_BASE + GICD_CTLR, YES);

			UInt32 gicc_ctlr = HAL::hal_mmio_read<UInt32>(GICC_BASE + GICC_CTLR);

			const auto kEnableSignalInt = YES;

			gicc_ctlr |= kEnableSignalInt;		  // Enable signaling of interrupts
			gicc_ctlr |= (kEnableSignalInt << 1); // Allow Group 1 interrupts in EL0

			HAL::hal_mmio_write(GICC_BASE + GICC_CTLR, gicc_ctlr);

			// Set priority mask (accept all priorities)
			HAL::hal_mmio_write(GICC_BASE + GICC_PMR, 0xFF);

			UInt32 icfgr = HAL::hal_mmio_read<UInt32>(GICD_BASE + GICD_ICFGR + (32 / 16) * 4);

			icfgr |= (0x2 << ((32 % 16) * 2)); // Edge-triggered
			HAL::hal_mmio_write(GICD_BASE + GICD_ICFGR + (32 / 16) * 4, icfgr);

			// Target interrupt 32 to CPU 1
			HAL::hal_mmio_write(GICD_BASE + GICD_ITARGETSR + (32 / 4) * 4, 0x2 << ((32 % 4) * 8));

			// Set interrupt 32 priority to lowest (0xFF)
			HAL::hal_mmio_write(GICD_BASE + GICD_IPRIORITYR + (32 / 4) * 4, 0xFF << ((32 % 4) * 8));

			// Enable interrupt 32 for AP.
			HAL::hal_mmio_write(GICD_BASE + GICD_ISENABLER + (32 / 32) * 4, 0x01 << (32 % 32));

			kout << "AP's GIC configured in ISR 32." << endl;
		}

		BOOL mp_handle_gic_interrupt_el0(Void)
		{
			// Read the interrupt ID
			UInt32 interrupt_id = HAL::hal_mmio_read<UInt32>(GICC_BASE + GICC_IAR);

			// Check if it's a valid interrupt (not spurious)
			if ((interrupt_id & 0x3FF) < 1020)
			{
				kout << "Handling interrupt for AP: " << (interrupt_id & 0x3FF) << endl;

				// TODO: Handle code here.

				// End the interrupt

				HAL::hal_mmio_write(GICC_BASE + GICC_EOIR, interrupt_id);

				return YES;
			}

			// spurious interrupt
			return NO;
		}
	} // namespace Detail

	EXTERN_C HAL::StackFramePtr mp_get_current_context(ProcessID pid)
	{
		return kProcessBlocks[pid % kSchedProcessLimitPerTeam].mFrame;
	}

	EXTERN_C Bool mp_register_process(HAL::StackFramePtr stack_frame, ProcessID pid)
	{
		MUST_PASS(stack_frame);

		const auto process_index = pid % kSchedProcessLimitPerTeam;

		kProcessBlocks[process_index].mFrame = stack_frame;

		return YES;
	}

	BOOL mp_initialize_gic(Void)
	{
		if (!Detail::kGICEnabled)
		{
			Detail::kGICEnabled = YES;
			Detail::mp_setup_gic_el0();

			return YES;
		}

		return NO;
	}
} // namespace Kernel