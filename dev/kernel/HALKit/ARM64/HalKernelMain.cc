/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <CFKit/Property.h>
#include <FirmwareKit/Handover.h>
#include <HALKit/ARM64/Processor.h>
#include <KernelKit/CodeMgr.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/MemoryMgr.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NetworkKit/IPC.h>
#include <NewKit/Json.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/CoreGfx/CoreGfx.h>

#include <HALKit/ARM64/ApplicationProcessor.h>

EXTERN_C void hal_init_platform(Kernel::HEL::BootInfoHeader* handover_hdr) {
  /************************************************** */
  /*     INITIALIZE AND VALIDATE HEADER.              */
  /************************************************** */

  kHandoverHeader = handover_hdr;

  if (kHandoverHeader->f_Magic != kHandoverMagic &&
      kHandoverHeader->f_Version != kHandoverVersion) {
    return;
  }

  /************************************** */
  /*     INITIALIZE BIT MAP.              */
  /************************************** */

  kKernelBitMpSize  = kHandoverHeader->f_BitMapSize;
  kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
      reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

  /// @note do initialize the interrupts after it.

  Kernel::mp_initialize_gic();

  /// after the scheduler runs, we must look over teams, every 5000s in order to schedule every
  /// process according to their affinity fairly.

  auto constexpr kSchedTeamSwitchMS = 5U;  /// @brief Team switch time in milliseconds.

  Kernel::HardwareTimer timer(rtl_milliseconds(kSchedTeamSwitchMS));

  STATIC Kernel::Array<UserProcessTeam, kSchedTeamCount> kTeams;

  SizeT team_index = 0U;

  /// @brief This just loops over the teams and switches between them.
  /// @details Not even round-robin, just a simple loop in this boot core we're at.
  while (YES) {
    if (team_index > (kSchedTeamCount - 1)) {
      team_index = 0U;
    }

    while (!UserProcessScheduler::The().SwitchTeam(kTeams[team_index]))
      ;

    timer.Wait();

    ++team_index;
  }
}
