// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef POWER_POWERFACTORY_H
#define POWER_POWERFACTORY_H

#include <KernelKit/DebugOutput.h>
#include <NeKit/Config.h>
#include <NeKit/ErrorOr.h>
#include <NeKit/Ref.h>
#include <modules/ACPI/ACPI.h>

#ifndef NE_POWER_FACTORY
#define NE_POWER_FACTORY : public PowerFactory
#endif

namespace Ne::Kernel {

enum class PowerState : UInt8 {
  kPowerStateOff = 0,
  kPowerStateOn  = 100,
  kPowerStateSleep = 101,
  kPowerStateHibernate = 102,
};

enum class PowerStdKind : UInt8 {
  kPowerStdKindUnknown = 0,
  kPowerStdKindACPI = 99,
  kPowerStdKindBCSA = 100,
  kPowerStdKindACPI2 = 102,
  kPowerStdKindLegacy = 103,
};

/// @brief Power Factory interface for ACPI or BCSA.
class PowerFactory {
 public:
  explicit PowerFactory() = default;
  virtual ~PowerFactory() = default;

  NE_COPY_DEFAULT(PowerFactory)

 public:
  virtual Bool Shutdown() { return NO; }  // shutdown
  virtual Void Reboot() {}                // soft-reboot
};

using LegacyPowerFactory = PowerFactory;
using UnknownPowerFactory = PowerFactory;

}  // namespace Ne::Kernel

#endif
