/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <misc/BenchKit/Chronometer.h>

namespace Kernel {
struct HWChronoTraits;

template <typename ChronoTraits = HWChronoTraits>
class HWChrono;

/// @brief BenchKit chrono logic for x64.
struct HWChronoTraits final {
 private:
  STATIC UInt64 TickImpl_(void) {
#ifdef __NE_AMD64__
    UInt64 a = 0, d = 0;

    asm volatile("rdtsc" : "=a"(a), "=d"(d));

    return (d << 32) | a;
#elif defined(__NE_ARM64__)
    UInt64 result;

    asm volatile("mrs %0, cntvct_el1" : "=r"(result));

    return result;
#else
#error !!! no backend defined !!!
#endif
  }

  friend HWChrono<HWChronoTraits>;
};

/// @brief hardware chronometer implementation using a trait to extract the data.
template <typename ChronoTraits>
class HWChrono BENCHKIT_INTERFACE {
 public:
  HWChrono()           = default;
  virtual ~HWChrono() override = default;

  NE_COPY_DEFAULT(HWChrono)

 public:
  Void Start() override { fStart = ChronoTraits::TickImpl_(); }

  Void Stop() override { fStop = ChronoTraits::TickImpl_(); }

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
