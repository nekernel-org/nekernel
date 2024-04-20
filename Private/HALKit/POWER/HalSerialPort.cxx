/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

using namespace NewOS;

/// @brief Writes to COM1.
/// @param bytes
void ke_io_write(const Char* bytes) {
  if (!bytes) return;

  SizeT index = 0;
  SizeT len = rt_string_len(bytes, 256);

  while (index < len) {
    // TODO
    ++index;
  }
}
