/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_SMP_MANAGER_HPP
#define _INC_SMP_MANAGER_HPP

#include <ArchKit/Arch.hpp>
#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/Ref.hpp>

#define kMaxHarts 8

namespace HCore {
using ThreadID = UInt32;

enum ThreadKind {
  kSystemReserved,  // System reserved thread, well user can't use it
  kStandard,        // user thread, cannot be used by kernel
  kFallback,  // fallback thread, cannot be used by user if not clear or used by
              // kernel.
  kBoot,      // The core we booted from, the mama.
};

///
/// \name HardwareThread
/// @brief CPU Hardware Thread (PowerPC, Intel, or NewCPU)
///

class HardwareThread final {
 public:
  explicit HardwareThread();
  ~HardwareThread();

 public:
  HCORE_COPY_DEFAULT(HardwareThread)

 public:
  operator bool();

 public:
  void Wake(const bool wakeup = false) noexcept;
  void Busy(const bool busy = false) noexcept;

 public:
  bool Switch(HAL::StackFrame* stack);
  bool IsWakeup() noexcept;

 public:
  HAL::StackFrame* StackFrame() noexcept;
  const ThreadKind& Kind() noexcept;
  bool IsBusy() noexcept;
  const ThreadID& ID() noexcept;

 private:
  HAL::StackFrame* m_Stack;
  ThreadKind m_Kind;
  ThreadID m_ID;
  bool m_Wakeup;
  bool m_Busy;
  Int64 m_PID;

 private:
  friend class SMPManager;
};

///
/// \name SMPManager
/// @brief Multi processor manager to manage other cores and dispatch tasks.
///

class SMPManager final {
 private:
  explicit SMPManager();

 public:
  ~SMPManager();

 public:
  HCORE_COPY_DEFAULT(SMPManager);

 public:
  bool Switch(HAL::StackFrame* the);
  HAL::StackFramePtr GetStackFrame() noexcept;

 public:
  Ref<HardwareThread> operator[](const SizeT& idx);
  bool operator!() noexcept;
  operator bool() noexcept;

 public:
  /// @brief Shared instance of the SMP Manager.
  /// @return the reference to the smp manager.
  static Ref<SMPManager> Shared();

 private:
  Array<HardwareThread, kMaxHarts> m_ThreadList;
  ThreadID m_CurrentThread;
};

// @brief wakes up thread.
// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack);

// @brief makes thread sleep.
// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack);
}  // namespace HCore

#endif  // !_INC_SMP_MANAGER_HPP
