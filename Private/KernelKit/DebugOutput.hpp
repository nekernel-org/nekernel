/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

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

  static TerminalDevice &Shared() noexcept;
};

inline TerminalDevice &EndLine() {
  TerminalDevice &selfTerm = TerminalDevice::Shared();
  selfTerm << "\n";
  return selfTerm;
}

namespace Detail {
bool serial_init();
}
}  // namespace HCore

#ifdef kcout
#undef kcout
#endif  // ifdef kcout

#define kcout TerminalDevice::Shared()
