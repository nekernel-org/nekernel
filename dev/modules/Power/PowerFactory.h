/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.h>
#include <NeKit/Defines.h>
#include <NeKit/ErrorOr.h>
#include <NeKit/Ref.h>
#include <modules/ACPI/ACPI.h>

#define NE_POWER_FACTORY : public PowerFactory

namespace Kernel {
class PowerFactory;

class PowerFactory {
 public:
  explicit PowerFactory() = default;
  virtual ~PowerFactory() = default;

  PowerFactory& operator=(const PowerFactory&) = default;
  PowerFactory(const PowerFactory&)            = default;

 public:
  Bool Shutdown() { return NO; };  // shutdown
  Void Reboot(){};                 // soft-reboot
};
}  // namespace Kernel