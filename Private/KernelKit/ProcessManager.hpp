/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __PROCESS_MANAGER__
#define __PROCESS_MANAGER__

#include <ArchKit/Arch.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/PermissionSelector.hxx>
#include <NewKit/LockDelegate.hpp>
#include <NewKit/MutableArray.hpp>
#include <NewKit/UserHeap.hpp>

#define kMinMicroTime AffinityKind::kStandard

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace HCore {
//! @brief Process identifier.
typedef Int64 ProcessID;

//! @brief Process name length.
inline constexpr SizeT kProcessLen = 256U;

//! @brief Forward declaration.
class Process;
class ProcessManager;
class ProcessHelper;

//! @brief Process status enum.
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
  kStandard = 150,
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

// @brief Process header structure.
class Process final {
 public:
  explicit Process(VoidPtr startImage = nullptr) : Image(startImage) {
    MUST_PASS(startImage);
  }
  ~Process() = default;

  HCORE_COPY_DEFAULT(Process)

 public:
  void AssignStart(UIntPtr &imageStart) noexcept;

 public:
  Char Name[kProcessLen] = {"HCore Process"};
  ProcessSubsystem SubSystem;
  ProcessSelector Selector;
  HAL::StackFrame *StackFrame{nullptr};
  AffinityKind Affinity;
  ProcessStatus Status;

  // Memory, images.
  HeapPtr PoolCursor{nullptr};
  ImagePtr Image{nullptr};
  HeapPtr Pool{nullptr};

  // memory usage
  SizeT UsedMemory{0};
  SizeT FreeMemory{0};

  ProcessTime PTime;
  PID ProcessId{-1};
  Int32 Ring{3};

 public:
  //! @brief boolean operator, check status.
  operator bool() { return Status != ProcessStatus::kDead; }

  //! @brief Crash program, exits with code ~0.
  void Crash();

  //! @brief Exits program.
  void Exit(Int32 exit_code = 0);

  //! @brief TLS Allocate
  VoidPtr New(const SizeT &sz);

  //! @brief TLS Free.
  Boolean Delete(VoidPtr ptr, const SizeT &sz);

  //! @brief Process name getter, example: "C RunTime"
  const Char *GetName();

  //! @brief Wakes up threads.
  void Wake(const bool wakeup = false);

 public:
  const ProcessSelector &GetSelector();
  const ProcessStatus &GetStatus();
  const AffinityKind &GetAffinity();

 private:
  friend ProcessManager;
  friend ProcessHelper;
};

using ProcessPtr = Process *;

//! @brief Kernel scheduler..
class ProcessManager final {
 private:
  explicit ProcessManager() = default;

 public:
  ~ProcessManager() = default;

  HCORE_COPY_DEFAULT(ProcessManager)

  operator bool() { return m_Headers.Count() > 0; }
  bool operator!() { return m_Headers.Count() == 0; }

  bool Add(Ref<Process> &Header);
  bool Remove(SizeT Header);

  Ref<Process> &GetCurrent();
  SizeT Run() noexcept;

  static Ref<ProcessManager> Shared();

 private:
  MutableArray<Ref<Process>> m_Headers;
  Ref<Process> m_CurrentProcess;
};

/*
 * Just a helper class, which contains some utilities for the scheduler.
 */

class ProcessHelper final {
 public:
  static bool Switch(HAL::StackFrame *newStack, const PID &newPid);
  static bool CanBeScheduled(Ref<Process> &process);
  static PID &GetCurrentPID();
  static bool StartScheduling();
};

const Int32 &rt_get_exit_code() noexcept;
}  // namespace HCore

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* ifndef __PROCESS_MANAGER__ */
