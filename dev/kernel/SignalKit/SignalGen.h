/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

#define SIGKILL 1
#define SIGPAUS 2
#define SIGEXEC 3
#define SIGTRAP 4
#define SIGABRT 5
#define SIGCONT 6
#define SIGSEG 7

#define SIGBREK 660
#define SIGATCH 661
#define SIGDTCH 662

/// @author Amlal El Mahrouss
/// @brief Signal Generation API.

namespace Kernel {
typedef UInt32 rt_signal_kind;

/// @brief Standard signal seed for general purpose usage.
inline static constexpr auto kBasicSignalSeed = 0x0895034f;

/// @brief Generate signal from **Sig**
template <rt_signal_kind Sig, SizeT Seed = kBasicSignalSeed>
inline rt_signal_kind sig_generate_unique() {
  static_assert(Sig > 0, "Signal is zero (invalid)");
  return Sig ^ Seed;
}

/// @brief Validate signal from **sig**
constexpr BOOL sig_validate_unique(rt_signal_kind sig) {
  return sig > 0;
}
}  // namespace Kernel
