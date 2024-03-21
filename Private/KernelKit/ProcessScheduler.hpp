/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __PROCESS_SCHEDULER__
#define __PROCESS_SCHEDULER__

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/LockDelegate.hpp>
#include <KernelKit/PermissionSelector.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/MutableArray.hpp>

#define kMinMicroTime AffinityKind::kHartStandard
#define kPIDInvalid (-1)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace HCore {
class ProcessHeader;
class ProcessTeam;
class ProcessManager;

//! @brief ProcessHeader identifier.
typedef Int64 ProcessID;

//! @brief ProcessHeader name length.
inline constexpr SizeT kProcessLen = 256U;

//! @brief Forward declaration.
class ProcessHeader;
class ProcessManager;
class ProcessHelper;

//! @brief ProcessHeader status enum.
enum class ProcessStatus : Int32 {
  kStarting,
  kRunning,
  kKilled,
  kFrozen,
  kDead
};

//! @brief Affinity is the amount of nano-seconds this process is going
//! to run.
enum class AffinityKind : Int32 {
  kInvalid = 300,
  kVeryHigh = 250,
  kHigh = 200,
  kHartStandard = 150,
  kLowUsage = 100,
  kVeryLowUsage = 50,
};

// operator overloading.

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

// end of operator overloading.

using ProcessSubsystem = UInt32;
using ProcessTime = UInt64;
using PID = Int64;

// for permission manager, tells where we run the code.
enum class ProcessSelector : Int {
  kRingUser,   /* user ring (or ring 3 in x86) */
  kRingDriver, /* ring 2 in x86, hypervisor privileges in other archs */
  kRingKernel, /* machine privileges */
};

// Helper types.
using ImagePtr = VoidPtr;
using HeapPtr = VoidPtr;

// @name ProcessHeader
// @brief Process Header (PH)
// Holds information about the running process.
// Thread execution is being abstracted away.
class ProcessHeader final {
 public:
  explicit ProcessHeader(VoidPtr startImage = nullptr) : Image(startImage) {
    MUST_PASS(startImage);
  }

  ~ProcessHeader() = default;

  HCORE_COPY_DEFAULT(ProcessHeader)

 public:
  void SetStart(UIntPtr &imageStart) noexcept;

 public:
  Char Name[kProcessLen] = {"HCore Process"};
  ProcessSubsystem SubSystem{0};
  ProcessSelector Selector{ProcessSelector::kRingUser};
  HAL::StackFramePtr StackFrame{nullptr};
  AffinityKind Affinity;
  ProcessStatus Status;

  // Memory, images.
  HeapPtr HeapCursor{nullptr};
  ImagePtr Image{nullptr};
  HeapPtr HeapPtr{nullptr};

  // memory usage
  SizeT UsedMemory{0};
  SizeT FreeMemory{0};

  enum {
    ExecutableType,
    DLLType,
    DriverType,
    TypeCount,
  };

  ProcessTime PTime;
  PID ProcessId{-1};
  Int32 Ring{3};
  Int32 Kind{0};

 public:
  //! @brief boolean operator, check status.
  operator bool() { return Status != ProcessStatus::kDead; }

  //! @brief Crash app, exits with code ~0.
  void Crash();

  //! @brief Exits app.
  void Exit(Int32 exitCode = 0);

  //! @brief TLS Allocate
  VoidPtr New(const SizeT &sz);

  //! @brief TLS Free.
  Boolean Delete(VoidPtr ptr, const SizeT &sz);

  //! @brief Wakes up threads.
  void Wake(const bool wakeup = false);

  // ProcessHeader getters.
 public:
  //! @brief ProcessHeader name getter, example: "C RunTime"
  const Char *GetName();

  const ProcessSelector &GetSelector();
  const ProcessStatus &GetStatus();
  const AffinityKind &GetAffinity();

 private:
  friend ProcessManager;
  friend ProcessHelper;
};

/// \brief Processs Team (contains multiple processes inside it.)
/// Equivalent to a process batch
class ProcessTeam final {
 public:
  explicit ProcessTeam() = default;
  ~ProcessTeam() = default;

  HCORE_COPY_DEFAULT(ProcessTeam);

  MutableArray<Ref<ProcessHeader>> &AsArray();
  Ref<ProcessHeader> &AsRef();

 public:
  MutableArray<Ref<ProcessHeader>> mProcessList;
  Ref<ProcessHeader> mCurrentProcess;
};

using ProcessPtr = ProcessHeader *;

/// @brief ProcessHeader manager class.
/// The main class which you call to schedule an app.
class ProcessManager final {
 private:
  explicit ProcessManager() = default;

 public:
  ~ProcessManager() = default;

  HCORE_COPY_DEFAULT(ProcessManager)

  operator bool() { return mTeam.AsArray().Count() > 0; }
  bool operator!() { return mTeam.AsArray().Count() == 0; }

  ProcessTeam &CurrentTeam() { return mTeam; }

  SizeT Add(Ref<ProcessHeader> &headerRef);
  bool Remove(SizeT headerIndex);

  Ref<ProcessHeader> &GetCurrent();
  SizeT Run() noexcept;

  static Ref<ProcessManager> Shared();

 private:
  ProcessTeam mTeam;
};

/*
 * Just a helper class, which contains some utilities for the scheduler.
 */

class ProcessHelper final {
 public:
  static bool Switch(HAL::StackFrame *newStack, const PID &newPid);
  static bool CanBeScheduled(Ref<ProcessHeader> &process);
  static PID &GetCurrentPID();
  static bool StartScheduling();
};

const Int32 &rt_get_exit_code() noexcept;
}  // namespace HCore

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef __PROCESS_SCHEDULER__ */
