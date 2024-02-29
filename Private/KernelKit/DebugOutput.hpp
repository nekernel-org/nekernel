/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/Stream.hpp>

#include "CompilerKit/CompilerKit.hpp"

namespace HCore {
// @brief Emulates a VT100 terminal.
class TerminalDevice final : public DeviceInterface<const Char *> {
 public:
  TerminalDevice(void (*print)(const Char *), void (*get)(const Char *))
      : DeviceInterface<const Char *>(print, get) {}

  virtual ~TerminalDevice() {}

  /// @brief returns device name (terminal name)
  /// @return string type (const char*)
  virtual const char *Name() const override { return ("TerminalDevice"); }

  HCORE_COPY_DEFAULT(TerminalDevice);

  static TerminalDevice Shared() noexcept;
};

inline TerminalDevice end_line() {
  TerminalDevice selfTerm = TerminalDevice::Shared();
  selfTerm << "\n";
  return selfTerm;
}
}  // namespace HCore

#ifdef kcout
#undef kcout
#endif  // ifdef kcout

#define kcout TerminalDevice::Shared()
#define endl end_line()

