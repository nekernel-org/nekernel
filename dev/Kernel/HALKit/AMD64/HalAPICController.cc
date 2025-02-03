/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <HALKit/AMD64/Processor.h>

#define cIOAPICRegVal (4)
#define cIOAPICRegReg (0)

namespace Kernel::HAL
{
	/// @brief Read from APIC controller.
	/// @param reg register.
	UInt32 APICController::Read(UInt32 reg) noexcept
	{
		MUST_PASS(this->fApic);

		UInt32 volatile* io_apic = (UInt32 volatile*)this->fApic;
		io_apic[cIOAPICRegReg]	 = (reg & 0xFF);

		return io_apic[cIOAPICRegVal];
	}

	/// @brief Write to APIC controller.
	/// @param reg register.
	/// @param value value.
	Void APICController::Write(UInt32 reg, UInt32 value) noexcept
	{
		MUST_PASS(this->fApic);

		UInt32 volatile* io_apic = (UInt32 volatile*)this->fApic;

		io_apic[cIOAPICRegReg] = (reg & 0xFF);
		io_apic[cIOAPICRegVal] = value;
	}
} // namespace Kernel::HAL
