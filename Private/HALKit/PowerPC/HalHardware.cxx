/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <HALKit/PowerPC/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

namespace NewOS {
namespace HAL {
UIntPtr hal_alloc_page(bool rw, bool user) { return 0; }
}  // namespace HAL

/// @brief wakes up thread.
/// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack) {}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack) {}

void ke_com_print(const Char* bytes) {
  if (!bytes) return;

  SizeT index = 0;
  SizeT len = rt_string_len(bytes, 256);

  while (index < len) {
    // TODO
    ++index;
  }
}
}  // namespace NewOS
