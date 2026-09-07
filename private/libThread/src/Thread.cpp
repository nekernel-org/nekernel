// SPDX-License-Identifier: Apache-2.0
// Copyright 2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-foss/ne-kernel

#include <SystemKit/Err.h>
#include <ThreadKit/Thread.h>

#ifdef kThreadMapMax
#undef kThreadMapMax
#endif

#define kThreadMapMax (1024UL)

#ifdef kThreadBaseHash
#undef kThreadBaseHash
#endif

#define kThreadBaseHash (0x5555ffff6ULL)

#ifdef kThreadSemID
#undef kThreadSemID
#endif

#define kThreadSemID "__NE_RUN_THREAD_HANDLE"

/// @brief The registered thread for a specific process.
static ThreadRef kThreadMap[kThreadMapMax];

/// @brief Exit codes as we want to track them.
static SInt32 kThreadExitCodes[kThreadMapMax];

/// @brief This hash-list stores the thread to be run.
static ThreadRef kThreadHeads[kThreadMapMax];

/// @brief This hash-list stores the thread to be run.
static SInt32 kThreadFlags[kThreadMapMax];

/// @brief Current thread reference
static ThreadRef kCurrentThread;

static __THREAD_UNSAFE Void _ThrRunThread(SInt32 argument_count, VoidPtr args,
                                          ThrProcKind procedure, ThreadRef ref) {
  static auto         kSemWaitTime = 1000;
  static SemaphoreRef sem_ref = ::SemCreate(0, kSemWaitTime, kThreadSemID);

  if (!sem_ref) return;

  auto ret                                      = procedure(argument_count, (Char**) args);
  kThreadExitCodes[kThreadMapMax % ref->__hash] = ret;

  if (ref == kCurrentThread) kCurrentThread = nullptr;

  ::SemClose(sem_ref);
  sem_ref = nullptr;
}

IMPORT_C ThreadRef ThrCreateThread(const Char* thread_name, ThrProcKind procedure,
                                   SInt32 argument_count, VoidPtr args, SInt32 flags) {
  ThreadRef ref = new REF_TYPE;
  if (!ref) return nullptr;

  if (!thread_name || !procedure) {
    delete ref;
    ref = nullptr;
    return nullptr;
  }

  ref->__hash = *(UInt32*) thread_name;
  ref->__hash += kThreadBaseHash;  // pad hash with a seed.

  ref->__self = (VoidPtr) procedure;

  kThreadMap[kThreadMapMax % ref->__hash]   = ref;
  kThreadHeads[kThreadMapMax % ref->__hash] = ref;
  kThreadFlags[kThreadMapMax % ref->__hash] = flags;

  kCurrentThread = ref;

  ::_ThrRunThread(argument_count, args, procedure, ref);

  return ref;
}

IMPORT_C SInt32 ThrExitThread(_Input ThreadRef ref, _Input SInt32 exit_code) {
  if (!ref) return kErrorInvalidData;

  kThreadMap[kThreadMapMax % ref->__hash]   = nullptr;
  kThreadHeads[kThreadMapMax % ref->__hash] = nullptr;

  kThreadFlags[kThreadMapMax % ref->__hash] = 0;

  if (kCurrentThread == ref) kCurrentThread = nullptr;

  kThreadExitCodes[kThreadMapMax % ref->__hash] = exit_code;

  return kErrorSuccess;
}