/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>
#include <NeKit/ErrorOr.h>

/// @file CoreProcessScheduler.h
/// @brief Core Process Scheduler header file.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#define kSchedMinMicroTime (AffinityKind::kStandard)
#define kSchedInvalidPID (-1)
#define kSchedProcessLimitPerTeam (32U)
#define kSchedTeamCount (256U)

#define kSchedMaxMemoryLimit (gib_cast(128)) /* max physical memory limit */
#define kSchedMaxStackSz (kib_cast(8))       /* maximum stack size */

#define kSchedNameLen (128U)

EXTERN_C void sched_idle_task(void);

namespace Kernel {
class USER_PROCESS;
class KERNEL_TASK;
class KernelTaskScheduler;
class UserProcessScheduler;
class UserProcessTeam;

template <typename T>
struct PROCESS_HEAP_TREE;

template <typename T>
struct PROCESS_SPECIAL_TREE;

template <typename T>
struct PROCESS_FILE_TREE;

enum {
  kInvalidTreeKind = 0U,
  kRedTreeKind     = 100U,
  kBlackTreeKind   = 101U,
  kTreeKindCount   = 3U,
};

template <typename T>
struct PROCESS_HEAP_TREE {
  static constexpr auto kHeap    = true;
  static constexpr auto kFile    = false;
  static constexpr auto kSpecial = false;

  T     Entry{nullptr};
  SizeT EntrySize{0UL};
  SizeT EntryPad{0UL};

  UInt32 Color{kBlackTreeKind};

  struct PROCESS_HEAP_TREE<T>* Parent {
    nullptr
  };
  struct PROCESS_HEAP_TREE<T>* Child {
    nullptr
  };

  struct PROCESS_HEAP_TREE<T>* Prev {
    nullptr
  };
  struct PROCESS_HEAP_TREE<T>* Next {
    nullptr
  };
};

template <typename T>
struct PROCESS_FILE_TREE {
  static constexpr auto kHeap    = false;
  static constexpr auto kFile    = true;
  static constexpr auto kSpecial = false;

  T     Entry{nullptr};
  SizeT EntrySize{0UL};
  SizeT EntryPad{0UL};

  UInt32 Color{kBlackTreeKind};

  struct PROCESS_FILE_TREE<T>* Parent {
    nullptr
  };
  struct PROCESS_FILE_TREE<T>* Child {
    nullptr
  };

  struct PROCESS_FILE_TREE<T>* Prev {
    nullptr
  };
  struct PROCESS_FILE_TREE<T>* Next {
    nullptr
  };
};

/***********************************************************************************/
/// @brief Subsystem enum type.
/***********************************************************************************/

enum class ProcessSubsystem : Int32 {
  kProcessSubsystemSecurity = 100,
  kProcessSubsystemUser,
  kProcessSubsystemService,
  kProcessSubsystemDriver,
  kProcessSubsystemInvalid = 0xFFFFFFF,
  kProcessSubsystemCount   = 4,
};

typedef UInt64 PTime;

/***********************************************************************************/
//! @brief Local Process identifier.
/***********************************************************************************/
typedef Int64 ProcessID;

/***********************************************************************************/
//! @brief Local Process status enum.
/***********************************************************************************/
enum class ProcessStatusKind : Int32 {
  kInvalid  = 0,
  kStarting = 100,
  kRunning,
  kKilled,
  kFrozen,
  kFinished,
  kCount = 6,
};

/***********************************************************************************/
//! @brief Affinity is the amount of nano-seconds this process is going to run.
/***********************************************************************************/
enum class AffinityKind : Int32 {
  kRealTime     = 100,
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
  Int32 lhs_int = static_cast<Int>(lhs);
  Int32 rhs_int = static_cast<Int>(rhs);

  return lhs_int < rhs_int;
}

inline bool operator>(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int>(lhs);
  Int32 rhs_int = static_cast<Int>(rhs);

  return lhs_int > rhs_int;
}

inline bool operator<=(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int>(lhs);
  Int32 rhs_int = static_cast<Int>(rhs);

  return lhs_int <= rhs_int;
}

inline bool operator>=(AffinityKind lhs, AffinityKind rhs) {
  Int32 lhs_int = static_cast<Int>(lhs);
  Int32 rhs_int = static_cast<Int>(rhs);

  return lhs_int >= rhs_int;
}

using ProcessTime = UInt64;
using PID         = Int64;

/***********************************************************************************/
/// @note For User manager, tells where we run the code.
/***********************************************************************************/
enum class ProcessLevelRing : Int32 {
  kRingStdUser   = 1,
  kRingSuperUser = 2,
  kRingGuestUser = 5,
  kRingCount     = 3,
};

/***********************************************************************************/
/// @brief Helper type to describe a code image.
/***********************************************************************************/
using ImagePtr = VoidPtr;

/***********************************************************************************/
/// @brief Helper class to contain a process's image and blob.
/***********************************************************************************/
struct PROCESS_IMAGE final {
  explicit PROCESS_IMAGE() = default;

 private:
  friend USER_PROCESS;
  friend KERNEL_TASK;

  friend class UserProcessScheduler;

  ImagePtr fCode;
  ImagePtr fBlob;

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
};
}  // namespace Kernel
