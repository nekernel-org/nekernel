/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

#define SIGKILL 1 /* kill */
#define SIGPAUS 2 /* pause */
#define SIGEXEC 3 /* execute */
#define SIGTRAP 4 /* trap */
#define SIGABRT 5 /* abort */
#define SIGCONT 6 /* continue */
#define SIGSEG 7  /* process fault */
#define SIGBREK 8
#define SIGATCH 9
#define SIGDTCH 10

/// @author Amlal El Mahrouss
/// @brief Signal Generation API.

namespace Kernel {
typedef SizeT rt_signal_kind;

/// @brief Standard signal seed for general purpose usage.
inline static constexpr auto kUserSignalSeed = 0x0895034fUL;

/// @brief Special signal seed for kernel usage.
inline static constexpr auto kKernelSignalSeed = 0x0895034f9fUL;

/// @brief Generate signal from **Sig**
template <rt_signal_kind Sig, SizeT Seed = kUserSignalSeed>
inline rt_signal_kind sig_generate_unique() {
  static_assert(Sig > 0, "Signal is zero (invalid)");
  return Sig ^ Seed;
}

/// @brief Checks if the signal matches the seed (user_seed or kernel_seed)
template <SizeT Seed>
inline BOOL sig_matches_seed(rt_signal_kind sig) {
  return (sig & 0xFF000000) == (Seed & 0xFF000000);
}

/// @brief Validate signal from **sig**
inline BOOL sig_validate_unique(rt_signal_kind sig) {
  return sig > 0;
}
}  // namespace Kernel
