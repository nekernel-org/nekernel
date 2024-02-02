/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

//! @file DebuggerPort.cxx
//! @brief UART debug via packets.

#include <ArchKit/Arch.hpp>

#define kDebugMaxPorts 16

#define kDebugUnboundPort 0xFFFF

#define kDebugMag0 'X'
#define kDebugMag1 'D'
#define kDebugMag2 'B'
#define kDebugMag3 'G'

#define kDebugSourceFile 0
#define kDebugLine 33
#define kDebugTeam 43
#define kDebugEOP 49

// after that we have start of additional data.

namespace HCore {
typedef Char rt_debug_type[255];

class DebuggerPorts final {
 public:
  Int16 fPort[kDebugMaxPorts];
  Int16 fBoundCnt;
};

void rt_debug_listen(DebuggerPorts* theHook) noexcept {
  if (theHook == nullptr) return;

  for (UInt32 i = 0U; i < kDebugMaxPorts; ++i) {
    HAL::Out16(theHook->fPort[i], kDebugMag0);
    HAL::rt_wait_for_io();

    HAL::Out16(theHook->fPort[i], kDebugMag1);
    HAL::rt_wait_for_io();

    HAL::Out16(theHook->fPort[i], kDebugMag2);
    HAL::rt_wait_for_io();

    HAL::Out16(theHook->fPort[i], kDebugMag3);
    HAL::rt_wait_for_io();

    if (HAL::In16(theHook->fPort[i] != kDebugUnboundPort)) theHook->fBoundCnt++;
  }
}
}  // namespace HCore
