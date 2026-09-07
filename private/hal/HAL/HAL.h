// SPDX-License-Identifier: Apache-2.0
// Copyright 2026, Ne.app, all rights reserved.
// Official repository: https://github.com/ne-app-open/krnl

#ifndef HAL_HAL_H
#define HAL_HAL_H

#include <ArchKit/ArchKit.h>

#ifndef HAL_UNUSED
#define HAL_UNUSED(X) (Void) X
#endif

#ifndef HAL_FINAL
#define HAL_FINAL final
#endif

#ifdef __cplusplus
namespace Ne::Kernel::HAL {
#endif

struct HAL_CALL_ENTRY;

#ifdef __cplusplus
using hal_proc_type = Ne::Kernel::Void (*)(Ne::Kernel::VoidPtr);
#else
typedef VoidPtr hal_proc_type;
#endif

enum : SInt64 { kAuthLevelInvalid, kAuthLevelHigh = 100, kAuthLevelMid, kAuthLevelLow };

/// @brief Entry structure of the HAL dispatch table.
/// @authors Amlal El Mahrouss (amlal@nekernel.org, amlal@ne-app.eu)

struct PACKED ALIGN(8) HAL_CALL_ENTRY HAL_FINAL {
  UInt64        fHash{};
  Bool          fActive{};
  hal_proc_type fProc;
  SInt64        fAuthLevel{};

#ifdef __cplusplus
  BOOL     IsActive() { return fActive; }
  explicit operator bool() { return fActive; }
#endif
};

#ifdef __cplusplus
EXTERN_C SInt32 hali_add_entry(HAL::hal_proc_type, const UInt64, const UInt64 hash);
EXTERN_C Void hali_remove_entry(const UInt64 hash);
#else
EXTERN_C SInt32 hali_add_entry(HAL::hal_proc_type, const UInt64, const SInt64 hash);
EXTERN_C Void hali_remove_entry(const SInt64 hash);
#endif

EXTERN_C UInt64 hali_hash_fnv64(const Char* path);

#ifdef __cplusplus
inline Array<HAL_CALL_ENTRY, kMaxDispatchCallCount> kRegisteredSystemCalls;
#endif

#ifdef __cplusplus
}  // namespace Ne::Kernel::HAL
#endif

#endif  // HAL_HAL_H