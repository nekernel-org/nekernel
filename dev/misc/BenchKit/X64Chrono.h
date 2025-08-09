/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <misc/BenchKit/Chrono.h>

#if defined(__NE_AMD64__)

namespace Kernel {
class X64Chrono;
struct X64ChronoTraits;

/// @brief BenchKit chrono logic for x64.
struct X64ChronoTraits {
 private:
  STATIC UInt64 TickImpl_(void) {
    UInt64 a = 0, d = 0;

    asm volatile("rdtsc" : "=a"(a), "=d"(d));
    return (d << 32) | a;
  }

  friend X64Chrono;
};

/// @brief X86_64 hardware chrono implementation using the `rdtsc` instruction.
class X64Chrono BENCHKIT_INTERFACE {
 public:
  X64Chrono()           = default;
  ~X64Chrono() override = default;

  NE_COPY_DEFAULT(X64Chrono);

 public:
  Void Start() override { fStart = X64ChronoTraits::TickImpl_(); }

  Void Stop() override { fStop = X64ChronoTraits::TickImpl_(); }

  Void Reset() override {
    fStart = 0;
    fStop  = 0;
  }

  UInt64 GetElapsedTime() const override { return fStop - fStart; }

 private:
  UInt64 fStart{};
  UInt64 fStop{};
};
}  // namespace Kernel

#endif  // defined(__NE_AMD64__)