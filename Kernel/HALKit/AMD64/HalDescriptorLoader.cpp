/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

namespace NewOS::HAL
{
	namespace Detail
	{
		STATIC RegisterGDT kRegGdt;
		STATIC HAL::Register64 kRegIdt;

		STATIC ::NewOS::Detail::AMD64::InterruptDescriptorAMD64
			kInterruptVectorTable[kKernelIdtSize];

		STATIC Void RemapPIC(Void) noexcept
		{
			// Remap PIC.
			HAL::Out8(0x20, 0x10 | 0x01);
			HAL::Out8(0xA0, 0x10 | 0x01);

			HAL::Out8(0x21, 32);
			HAL::Out8(0xA1, 40);

			HAL::Out8(0x21, 4);
			HAL::Out8(0xA1, 2);

			HAL::Out8(0x21, 0x01);
			HAL::Out8(0xA1, 0x01);

			HAL::Out8(0x21, 0x00);
			HAL::Out8(0xA1, 0x00);
		}
	} // namespace Detail

	/// @brief Loads the provided Global Descriptor Table.
	/// @param gdt
	/// @return
	Void GDTLoader::Load(RegisterGDT& gdt)
	{
		MUST_PASS(gdt.Base != 0);

		Detail::kRegGdt.Base  = gdt.Base;
		Detail::kRegGdt.Limit = gdt.Limit;

		hal_load_gdt(Detail::kRegGdt);
	}

	Void IDTLoader::Load(Register64& idt)
	{
		volatile ::NewOS::UIntPtr** baseIdt = (volatile ::NewOS::UIntPtr**)idt.Base;

		MUST_PASS(baseIdt);

		Detail::RemapPIC();

		for (UInt16 i = 0; i < kKernelIdtSize; ++i)
		{
			MUST_PASS(baseIdt[i]);

			Detail::kInterruptVectorTable[i].Selector		= kGdtCodeSelector;
			Detail::kInterruptVectorTable[i].Ist			= 0x0;
			Detail::kInterruptVectorTable[i].TypeAttributes = kInterruptGate;
			Detail::kInterruptVectorTable[i].OffsetLow		= ((UIntPtr)baseIdt[i] & 0xFFFF);
			Detail::kInterruptVectorTable[i].OffsetMid		= (((UIntPtr)baseIdt[i] >> 16) & 0xFFFF);
			Detail::kInterruptVectorTable[i].OffsetHigh =
				(((UIntPtr)baseIdt[i] >> 32) & 0xFFFFFFFF);
			Detail::kInterruptVectorTable[i].Zero = 0x0;
		}

		Detail::kRegIdt.Base  = reinterpret_cast<UIntPtr>(Detail::kInterruptVectorTable);
		Detail::kRegIdt.Limit = sizeof(::NewOS::Detail::AMD64::InterruptDescriptorAMD64) *
								(kKernelIdtSize - 1);

		hal_load_idt(Detail::kRegIdt);
	}

	void GDTLoader::Load(Ref<RegisterGDT>& gdt)
	{
		GDTLoader::Load(gdt.Leak());
	}

	void IDTLoader::Load(Ref<Register64>& idt)
	{
		IDTLoader::Load(idt.Leak());
	}
} // namespace NewOS::HAL
