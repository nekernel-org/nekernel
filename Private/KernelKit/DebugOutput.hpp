/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/Stream.hpp>

#define kDebugMaxPorts 16

#define kDebugUnboundPort 0x0FEED

#define kDebugMag0 'H'
#define kDebugMag1 'D'
#define kDebugMag2 'B'
#define kDebugMag3 'G'

#define kDebugSourceFile 0
#define kDebugLine 33
#define kDebugTeam 43
#define kDebugEOP 49

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

inline TerminalDevice carriage_return() {
  TerminalDevice selfTerm = TerminalDevice::Shared();
  selfTerm << "\r";
  return selfTerm;
}

inline TerminalDevice get_buffer(Char* buf) {
  TerminalDevice selfTerm = TerminalDevice::Shared();
  selfTerm >> buf;
  return selfTerm;
}

typedef Char rt_debug_type[255];

class DebuggerPorts final {
 public:
  Int16 fPort[kDebugMaxPorts];
  Int16 fBoundCnt;
};
}  // namespace HCore

#ifdef kcout
#undef kcout
#endif  // ifdef kcout

#define kcout TerminalDevice::Shared()
#define endl end_line()

