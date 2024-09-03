/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

namespace Kernel::HAL
{
	namespace Detail
	{
		STATIC ::Kernel::Detail::AMD64::InterruptDescriptorAMD64
			kInterruptVectorTable[kKernelIdtSize];

		STATIC Void hal_remap_intel_pic_ctrl(Void) noexcept
		{
			// Remap PIC.
			HAL::Out8(0x20, 0x11);
			HAL::Out8(0xA0, 0x11);

			HAL::Out8(0x21, 40);
			HAL::Out8(0xA1, 32);

			HAL::Out8(0x21, 4);
			HAL::Out8(0xA1, 2);

			HAL::Out8(0x21, 0x01);
			HAL::Out8(0xA1, 0x01);

			HAL::Out8(0x21, 0xFD);
			HAL::Out8(0xA1, 0xFF);
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
		volatile ::Kernel::UIntPtr** ptr_ivt = (volatile ::Kernel::UIntPtr**)idt.Base;

		for (UInt16 idt_indx = 0; idt_indx < 12; ++idt_indx)
		{
			MUST_PASS(ptr_ivt[idt_indx]);

			Detail::kInterruptVectorTable[idt_indx].Selector	   = kGdtKernelCodeSelector;
			Detail::kInterruptVectorTable[idt_indx].Ist			   = 0;
			Detail::kInterruptVectorTable[idt_indx].TypeAttributes = kTrapGate;
			Detail::kInterruptVectorTable[idt_indx].OffsetLow	   = ((UIntPtr)ptr_ivt[idt_indx] & __INT16_MAX__);
			Detail::kInterruptVectorTable[idt_indx].OffsetMid	   = (((UIntPtr)ptr_ivt[idt_indx] >> 16) & __INT16_MAX__);
			Detail::kInterruptVectorTable[idt_indx].OffsetHigh =
				(((UIntPtr)ptr_ivt[idt_indx] >> 32) & __INT32_MAX__);

			Detail::kInterruptVectorTable[idt_indx].Zero = 0x0;
		}

		for (UInt16 idt_indx = 13; idt_indx < kKernelIdtSize; ++idt_indx)
		{
			MUST_PASS(ptr_ivt[idt_indx]);

			Detail::kInterruptVectorTable[idt_indx].Selector	   = kGdtKernelCodeSelector;
			Detail::kInterruptVectorTable[idt_indx].Ist			   = 0;
			Detail::kInterruptVectorTable[idt_indx].TypeAttributes = kInterruptGate;
			Detail::kInterruptVectorTable[idt_indx].OffsetLow	   = ((UIntPtr)ptr_ivt[idt_indx] & __INT16_MAX__);
			Detail::kInterruptVectorTable[idt_indx].OffsetMid	   = (((UIntPtr)ptr_ivt[idt_indx] >> 16) & __INT16_MAX__);
			Detail::kInterruptVectorTable[idt_indx].OffsetHigh =
				(((UIntPtr)ptr_ivt[idt_indx] >> 32) & __INT32_MAX__);

			Detail::kInterruptVectorTable[idt_indx].Zero = 0x0;
		}

		hal_load_idt(idt);

		Detail::hal_remap_intel_pic_ctrl();
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
