/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/Timer.h>
#include <NewKit/KString.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/Utils.h>
#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel {
/// @brief Dumping factory class.
class RecoveryFactory final {
 public:
  STATIC Void Recover() noexcept;
};

/***********************************************************************************/
/// @brief Stops execution of the kernel.
/// @param id kernel stop ID.
/***********************************************************************************/
Void ke_panic(const Kernel::Int32& id, const Char* message) {
  (Void)(kout << "*** STOP ***\r");

  (Void)(kout << "Kernel_Panic_MSG: " << message << kendl);
  (Void)(kout << "Kernel_Panic_ID: " << hex_number(id) << kendl);
  (Void)(kout << "Kernel_Panic_CR2: " << hex_number((UIntPtr) hal_read_cr2()) << kendl);

  RecoveryFactory::Recover();
}

Void RecoveryFactory::Recover() noexcept {
  while (YES) {
    HAL::rt_halt();
  }
}

void ke_runtime_check(bool expr, const Char* file, const Char* line) {
  if (!expr) {
    (Void)(kout << "*** CHECK ***\r");

    (Void)(kout << "Kernel_Panic_FILE: " << file << kendl);
    (Void)(kout << "Kernel_Panic_LINE: " << line << kendl);

    ke_panic(RUNTIME_CHECK_FAILED, file);  // Runtime Check failed
  }
}
}  // namespace Kernel
