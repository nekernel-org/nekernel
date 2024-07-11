/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hxx>

namespace Kernel::HAL
{
	/// @brief Read from APIC controller.
	/// @param reg register.
	UInt32 APICController::Read(UInt32 reg) noexcept
	{
		MUST_PASS(this->fApic);

		UInt32 volatile* ioapic = (UInt32 volatile*)this->fApic;
		ioapic[0]				= (reg & 0xff);

		return ioapic[4];
	}

	/// @brief Write to APIC controller.
	/// @param reg register.
	/// @param value value.
	Void APICController::Write(UInt32 reg, UInt32 value) noexcept
	{
		MUST_PASS(this->fApic);

		UInt32 volatile* ioapic = (UInt32 volatile*)this->fApic;

		ioapic[0] = (reg & 0xFF);
		ioapic[4] = value;
	}
} // namespace Kernel::HAL
