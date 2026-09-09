// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef KERNELKIT_COREPROCESSSCHEDULER_H
#define KERNELKIT_COREPROCESSSCHEDULER_H

#include <NeKit/Config.h>
#include <NeKit/ErrorOr.h>

/// @file CoreProcessScheduler.h
/// @brief Core Process Scheduler header file.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#define kCPSMinMicroTime (AffinityKind::kStandard)
#define kCPSInvalidPID (-1)
#define kCPSProcessLimitPerTeam (32U)
#define kCPSTeamCount (256U)

#define kCPSMaxMemoryLimit (gib_cast(128)) /* max physical memory limit */
#define kCPSMaxStackSz (kib_cast(8))       /* maximum stack size */
#define kCPSNameLen (128U)

EXTERN_C void sched_idle_task(void);

namespace Ne::Kernel {

class UserProcess;
class KernelTask;
class KernelTaskScheduler;
class UserProcessScheduler;
class UserProcessTeam;

template <typename T>
struct ProcessHeapTree;

template <typename T>
struct ProcessSpecialTree;

template <typename T>
struct ProcessFileTree;

enum struct TreeKind : UInt32 {
  kInvalidTreeKind = 0U,
  kRedTreeKind     = 100U,
  kBlackTreeKind   = 101U,
  kTreeKindCount   = 3U,
};

template <typename T>
struct ProcessHeapTree final {
  static constexpr auto kHeap    = true;
  static constexpr auto kFile    = false;
  static constexpr auto kSpecial = false;

  T     Entry{nullptr};
  SizeT EntrySize{0UL};
  SizeT EntryPad{0UL};

  TreeKind Color{TreeKind::kBlackTreeKind};

  struct ProcessHeapTree<T>* Parent{nullptr};
  struct ProcessHeapTree<T>* Child{nullptr};

  struct ProcessHeapTree<T>* Prev{nullptr};
  struct ProcessHeapTree<T>* Next{nullptr};
};

template <typename T>
struct ProcessFileTree final {
  static constexpr auto kHeap    = false;
  static constexpr auto kFile    = true;
  static constexpr auto kSpecial = false;

  T     Entry{nullptr};
  SizeT EntrySize{0UL};
  SizeT EntryPad{0UL};

  TreeKind Color{TreeKind::kBlackTreeKind};

  struct ProcessFileTree<T>* Parent{nullptr};

  struct ProcessFileTree<T>* Child{nullptr};

  struct ProcessFileTree<T>* Prev{nullptr};

  struct ProcessFileTree<T>* Next{nullptr};
};

template <typename T>
struct ProcessSpecialTree final {
  static constexpr auto kHeap    = false;
  static constexpr auto kFile    = false;
  static constexpr auto kSpecial = true;

  T     Entry{nullptr};
  SizeT EntrySize{0UL};
  SizeT EntryPad{0UL};

  /// @brief a context is where the resource comes from.
  using ProcessCtx = UInt32;

  /// \note could be a socket, printer, device...
  ProcessCtx EntryContext{0UL};

  TreeKind Color{TreeKind::kBlackTreeKind};

  struct ProcessSpecialTree<T>* Parent{nullptr};

  struct ProcessSpecialTree<T>* Child{nullptr};

  struct ProcessSpecialTree<T>* Prev{nullptr};

  struct ProcessSpecialTree<T>* Next{nullptr};
};

/***********************************************************************************/
/// @brief Subsystem enum type.
/***********************************************************************************/

enum struct ProcessSubsystem : Int32 {
  kProcessSubsystemSecurity = 100,
  kProcessSubsystemUser,
  kProcessSubsystemService,
  kProcessSubsystemDriver,
  kProcessSubsystemKernel,
  kProcessSubsystemPOSIX,
  kProcessSubsystemNeSystem,
  kProcessSubsystemNeAnt,
  kProcessSubsystemWine64, /// @brief Wine Project Subsystem.
  kProcessSubsystemCount   = kProcessSubsystemWine64 - kProcessSubsystemSecurity + 1,
  kProcessSubsystemInvalid = 0xFFFFFFF,
};

/***********************************************************************************/
//! @brief Local Process status enum.
/***********************************************************************************/
enum struct ProcessStatusKind : Int32 {
  kInvalid  = 0,
  kStarting = 100,
  kRunning,
  kKilled,
  kFrozen,
  kFinished,
  kCount = kFinished - kStarting + 1,
};

/***********************************************************************************/
//! @brief Affinity is the amount of nano-seconds this process is going to run.
/***********************************************************************************/
enum struct AffinityKind : Int32 {
  kInvalid      = 0,
  kUltraHigh    = 100,
  kVeryHigh     = 150,
  kHigh         = 200,
  kStandard     = 1000,
  kLowUsage     = 1500,
  kVeryLowUsage = 2000,
};

/***********************************************************************************/
//! Operators for AffinityKind
/***********************************************************************************/

inline bool operator<(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int32>(lhs);
  Int32 rhs_int = static_cast<Int32>(rhs);

  return lhs_int < rhs_int;
}

inline bool operator>(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int32>(lhs);
  Int32 rhs_int = static_cast<Int32>(rhs);

  return lhs_int > rhs_int;
}

inline bool operator<=(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int>(lhs);
  Int32 rhs_int = static_cast<Int32>(rhs);

  return lhs_int <= rhs_int;
}

inline bool operator>=(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int32>(lhs);
  Int32 rhs_int = static_cast<Int32>(rhs);

  return lhs_int >= rhs_int;
}

using PTime       = UInt64;
using ProcessTime = PTime;

/***********************************************************************************/
//! @brief Local Process Identifier type.
/***********************************************************************************/
using ProcessID = Int64;

/***********************************************************************************/
/// @note For User manager, tells where we run the code.
/***********************************************************************************/
enum struct ProcessLevelRing : Int32 {
  kRingInvalid = -1,
  kRingStdUser   = 100,
  kRingSuperUser = 200,
  kRingGuestUser = 500,
  kRingCount     = 3,
};

/***********************************************************************************/
/// @brief Helper type to describe a code image.
/***********************************************************************************/
using ImagePtr = VoidPtr;

/***********************************************************************************/
/// @brief Helper class to contain a process's image and blob.
/***********************************************************************************/
struct ProcessImage final {
  explicit ProcessImage() = default;

 private:
  friend UserProcess;
  friend KernelTask;

  friend UserProcessScheduler;
  friend KernelTaskScheduler;

  ImagePtr fCode{};
  ImagePtr fBlob{};
  SizeT    fBlobSz{};

 public:
  Bool HasCode() const { return this->fCode != nullptr; }

  Bool HasImage() const { return this->fBlob != nullptr; }

  ErrorOr<ImagePtr> LeakImage() {
    if (this->fCode) {
      return ErrorOr<ImagePtr>{this->fCode};
    }

    return ErrorOr<ImagePtr>{kErrorInvalidData};
  }

  ErrorOr<ImagePtr> LeakBlob() {
    if (this->fBlob) {
      return ErrorOr<ImagePtr>{this->fBlob};
    }

    return ErrorOr<ImagePtr>{kErrorInvalidData};
  }

  SizeT LeakBlobSz() const { return this->fBlobSz; }
};

}  // namespace Ne::Kernel

#endif
