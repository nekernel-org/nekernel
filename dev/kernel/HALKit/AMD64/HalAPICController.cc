/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/AMD64/Processor.h>
#include <modules/ACPI/ACPIFactoryInterface.h>

#define kIOAPICRegVal (4)
#define kIOAPICRegReg (0)

namespace Kernel::HAL {
APICController::APICController(VoidPtr base) : fApic(base) {}

/// @brief Read from APIC controller.
/// @param reg register.
UInt32 APICController::Read(UInt32 reg) noexcept {
  MUST_PASS(this->fApic);

  UInt32 volatile* io_apic = (UInt32 volatile*) this->fApic;
  io_apic[kIOAPICRegReg]   = (reg & 0xFF);

  return io_apic[kIOAPICRegVal];
}

/// @brief Write to APIC controller.
/// @param reg register.
/// @param value value.
Void APICController::Write(UInt32 reg, UInt32 value) noexcept {
  MUST_PASS(this->fApic);

  UInt32 volatile* io_apic = (UInt32 volatile*) this->fApic;

  io_apic[kIOAPICRegReg] = (reg & 0xFF);
  io_apic[kIOAPICRegVal] = value;
}
}  // namespace Kernel::HAL
