/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/AMD64/Processor.h>
#include <modules/ACPI/ACPIFactoryInterface.h>

namespace Kernel::HAL {
/***********************************************************************************/
/// Constructors.
/***********************************************************************************/
LAPICDmaWrapper::LAPICDmaWrapper(VoidPtr base) : fApic(base) {}
LAPICDmaWrapper::~LAPICDmaWrapper() = default;

/***********************************************************************************/
/// @brief Read from APIC controller.
/// @param reg register.
/***********************************************************************************/
UInt32 LAPICDmaWrapper::Read(UInt16 reg) noexcept {
  MUST_PASS(this->fApic);

  UInt32 volatile* io_apic = (UInt32 volatile*) this->fApic;
  return io_apic[reg];
}

/***********************************************************************************/
/// @brief Write to APIC controller.
/// @param reg register.
/// @param value value.
/***********************************************************************************/
Void LAPICDmaWrapper::Write(UInt16 reg, UInt32 value) noexcept {
  MUST_PASS(this->fApic);

  UInt32 volatile* io_apic = (UInt32 volatile*) this->fApic;
  io_apic[reg]             = value;
}
}  // namespace Kernel::HAL
