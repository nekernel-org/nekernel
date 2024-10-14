/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <HALKit/AMD64/Processor.hxx>

namespace Kernel::HAL
{
	namespace Detail
	{
		STATIC ::Kernel::Detail::AMD64::InterruptDescriptorAMD64
			kInterruptVectorTable[kKernelIdtSize];

		STATIC Void hal_remap_intel_pic_ctrl(Void) noexcept
		{
			uint8_t a1_saved = In8(kPICData);
			uint8_t a2_saved = In8(kPIC2Data);

			Out8(kPICCommand, 0x11); // Start initialization
			Out8(kPICData, 0x20);	 // Master PIC offset
			Out8(kPICData, 0x04);	 // Tell master PIC there is a slave
			Out8(kPICData, 0x01);	 // 8086 mode

			Out8(kPIC2Command, 0x11); // Start initialization
			Out8(kPIC2Data, 0x28);	  // Slave PIC offset
			Out8(kPIC2Data, 0x02);	  // Tell slave PIC its cascade
			Out8(kPIC2Data, 0x01);	  // 8086 mode

			Out8(kPICData, a1_saved); // Restore saved masks
			Out8(kPIC2Data, a2_saved);
		}

		STATIC Void hal_enable_pit() noexcept
		{
			// Configure PIT to receieve scheduler interrupts.

			UInt32 cCommonDivisor = kPITFrequency / 100; // 100 Hz.

			HAL::Out8(kPITControlPort, 0x36);						  // Command to PIT
			HAL::Out8(kPITChannel0Port, cCommonDivisor & 0xFF);		  // Send low byte
			HAL::Out8(kPITControlPort, (cCommonDivisor >> 8) & 0xFF); // Send high byte
		}
	} // namespace Detail

	/// @brief Loads the provided Global Descriptor Table.
	/// @param gdt
	/// @return
	Void GDTLoader::Load(RegisterGDT& gdt)
	{
		hal_load_gdt(gdt);
	}

	Void IDTLoader::Load(Register64& idt)
	{
		Detail::hal_remap_intel_pic_ctrl();
		Detail::hal_enable_pit();

		volatile ::Kernel::UIntPtr** ptr_ivt = (volatile ::Kernel::UIntPtr**)idt.Base;

		for (UInt16 idt_indx = 0; idt_indx < (kKernelIdtSize); ++idt_indx)
		{
			Detail::kInterruptVectorTable[idt_indx].Selector	   = kIDTSelector;
			Detail::kInterruptVectorTable[idt_indx].Ist			   = 0;
			Detail::kInterruptVectorTable[idt_indx].TypeAttributes = kInterruptGate;
			Detail::kInterruptVectorTable[idt_indx].OffsetLow	   = ((UIntPtr)ptr_ivt[idt_indx] & 0xFFFF);
			Detail::kInterruptVectorTable[idt_indx].OffsetMid	   = (((UIntPtr)ptr_ivt[idt_indx] >> 16) & 0xFFFF);
			Detail::kInterruptVectorTable[idt_indx].OffsetHigh =
				(((UIntPtr)ptr_ivt[idt_indx] >> 32) & 0xFFFFFFFF);

			Detail::kInterruptVectorTable[idt_indx].Zero = 0x0;
		}

		idt.Base  = (UIntPtr)&Detail::kInterruptVectorTable;
		idt.Limit = sizeof(::Kernel::Detail::AMD64::InterruptDescriptorAMD64) *
					(kKernelIdtSize)-1;

		hal_load_idt(idt);
	}

	void GDTLoader::Load(Ref<RegisterGDT>& gdt)
	{
		GDTLoader::Load(gdt.Leak());
	}

	void IDTLoader::Load(Ref<Register64>& idt)
	{
		IDTLoader::Load(idt.Leak());
	}
} // namespace Kernel::HAL
