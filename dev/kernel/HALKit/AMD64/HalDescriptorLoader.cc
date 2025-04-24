/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <FSKit/NeFS.h>
#include <ArchKit/ArchKit.h>
#include <HALKit/AMD64/Processor.h>

#define kPITDefaultTicks (1000U)

namespace Kernel::HAL
{
	namespace Detail
	{
		STATIC ::Kernel::Detail::AMD64::InterruptDescriptorAMD64
			kInterruptVectorTable[kKernelIdtSize] = {};

#if 0
		STATIC void hal_set_irq_mask(UInt8 irql) [[maybe_unused]]
		{
			UInt16 port;
			UInt8  value;

			if (irql < 8)
			{
				port = kPICData;
			}
			else
			{
				port = kPIC2Data;
				irql -= 8;
			}

			value = rt_in8(port) | (1 << irql);
			rt_out8(port, value);
		}
#endif // make gcc shut up

		STATIC void hal_clear_irq_mask(UInt8 irql) [[maybe_unused]]
		{
			UInt16 port;
			UInt8  value;

			if (irql < 8)
			{
				port = kPICData;
			}
			else
			{
				port = kPIC2Data;
				irql -= 8;
			}

			value = rt_in8(port) & ~(1 << irql);
			rt_out8(port, value);
		}

		STATIC Void hal_enable_pit(UInt16 ticks) noexcept
		{
			if (ticks == 0)
				ticks = kPITDefaultTicks;

			// Configure PIT to receieve scheduler interrupts.

			UInt16 kPITCommDivisor = kPITFrequency / ticks; // 100 Hz.

			HAL::rt_out8(kPITControlPort, 0x36);						   // Command to PIT
			HAL::rt_out8(kPITChannel0Port, kPITCommDivisor & 0xFF);		   // Send low byte
			HAL::rt_out8(kPITChannel0Port, (kPITCommDivisor >> 8) & 0xFF); // Send high byte

			hal_clear_irq_mask(32);
		}
	} // namespace Detail

	/// @brief Loads the provided Global Descriptor Table.
	/// @param gdt
	/// @return
	Void GDTLoader::Load(Register64& gdt)
	{
		hal_load_gdt(gdt);
	}

	Void IDTLoader::Load(Register64& idt)
	{
		rt_cli();

		const Int16 kPITTickForScheduler = kPITDefaultTicks;

		volatile ::Kernel::UIntPtr** ptr_ivt = (volatile ::Kernel::UIntPtr**)idt.Base;

		for (SizeT idt_indx = 0; idt_indx < kKernelIdtSize; ++idt_indx)
		{
			Detail::kInterruptVectorTable[idt_indx].Selector	   = kIDTSelector;
			Detail::kInterruptVectorTable[idt_indx].Ist			   = 0;
			Detail::kInterruptVectorTable[idt_indx].TypeAttributes = kInterruptGate;
			Detail::kInterruptVectorTable[idt_indx].OffsetLow	   = ((UIntPtr)ptr_ivt[idt_indx] & 0xFFFF);
			Detail::kInterruptVectorTable[idt_indx].OffsetMid	   = (((UIntPtr)ptr_ivt[idt_indx] >> 16) & 0xFFFF);
			Detail::kInterruptVectorTable[idt_indx].OffsetHigh =
				(((UIntPtr)ptr_ivt[idt_indx] >> 32) & 0xFFFFFFFF);

			Detail::kInterruptVectorTable[idt_indx].Zero = 0;
		}

		idt.Base  = (UIntPtr)&Detail::kInterruptVectorTable[0];
		idt.Limit = sizeof(::Kernel::Detail::AMD64::InterruptDescriptorAMD64) *
					(kKernelIdtSize);

		Detail::hal_enable_pit(kPITTickForScheduler);

		hal_load_idt(idt);

		rt_sti();
	}

	/// @brief Loads the Global Descriptor Table into the CPU.
	/// @param gdt GDT register wrapped in a ref.
	void GDTLoader::Load(Ref<Register64>& gdt)
	{
		if (!gdt)
			return;

		GDTLoader::Load(gdt.Leak());
	}

	/// @brief Loads the IDT, for interupts.
	/// @param idt IDT register wrapped in a ref.
	void IDTLoader::Load(Ref<Register64>& idt)
	{
		if (!idt)
			return;

		IDTLoader::Load(idt.Leak());
	}
} // namespace Kernel::HAL
