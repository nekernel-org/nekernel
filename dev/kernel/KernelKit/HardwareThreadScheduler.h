/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __INC_MP_MANAGER_H__
#define __INC_MP_MANAGER_H__

#include <ArchKit/ArchKit.h>
#include <CompilerKit/CompilerKit.h>
#include <NeKit/Ref.h>

/// @note Last Rev Sun 28 Jul CET 2024
/// @note Last Rev Thu, Aug  1, 2024  9:07:38 AM

#define kMaxAPInsideSched (4U)

namespace Kernel {
class HardwareThread;
class HardwareThreadScheduler;

using ThreadID = UInt32;

enum ThreadKind {
  kAPInvalid        = 0,
  kAPSystemReserved = 100,  // System reserved thread, well user can't use it
  kAPStandard,              // user thread, cannot be used by Kernel
  kAPRealTime,              // fallback thread, cannot be used by user if not clear or
                            // used by Kernel.
  kAPBoot,                  // The core we booted from, the mama.
  kAPCount,
};

typedef enum ThreadKind ThreadKind;
typedef ThreadID        ThreadID;

/***********************************************************************************/
///
/// \name HardwareThread
/// \brief Abstraction over the CPU's core, used to run processes or threads.
///
/***********************************************************************************/

class HardwareThread final {
 public:
  explicit HardwareThread();
  ~HardwareThread();

 public:
  NE_COPY_DEFAULT(HardwareThread)

 public:
  operator bool();

 public:
  void Wake(const BOOL wakeup = false) noexcept;
  void Busy(const BOOL busy = false) noexcept;

 public:
  BOOL Switch(HAL::StackFramePtr frame);
  BOOL IsWakeup() noexcept;

 public:
  HAL::StackFramePtr StackFrame() noexcept;
  ThreadKind&        Kind() noexcept;
  BOOL               IsBusy() noexcept;
  ThreadID&          ID() noexcept;

 private:
  HAL::StackFramePtr fStack{nullptr};
  ThreadKind         fKind{ThreadKind::kAPStandard};
  ThreadID           fID{0};
  Bool               fWakeup{NO};
  Bool               fBusy{NO};
  UInt64             fPTime{0};

 private:
  friend class HardwareThreadScheduler;
  friend class UserProcessHelper;
};

///
/// \name HardwareThreadScheduler
/// \brief Class to manage the thread scheduling.
///

class HardwareThreadScheduler final : public ISchedulable {
 private:
  friend class UserProcessHelper;

 public:
  explicit HardwareThreadScheduler();
  ~HardwareThreadScheduler();
  NE_COPY_DEFAULT(HardwareThreadScheduler)

 public:
  HAL::StackFramePtr Leak() noexcept;

 public:
  Ref<HardwareThread*> operator[](SizeT idx);
  bool                 operator!() noexcept;
                       operator bool() noexcept;

  Bool IsUser() override { return Yes; }

  Bool IsKernel() override { return No; }

  Bool HasMP() override { return kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled; }

 public:
  /// @brief Shared instance of the MP Mgr.
  /// @return the reference to the mp manager class.
  STATIC HardwareThreadScheduler& The();

 public:
  /// @brief Returns the amount of threads present in the system.
  /// @returns SizeT the amount of cores present.
  SizeT Capacity() noexcept;

 private:
  Array<HardwareThread, kMaxAPInsideSched> fThreadList;
  ThreadID                                 fCurrentThread{0};
};

/// @brief wakes up thread.
/// wakes up thread from hang.
Void mp_wakeup_thread(HAL::StackFramePtr stack);

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
Void mp_hang_thread(HAL::StackFramePtr stack);
}  // namespace Kernel

#endif  // !__INC_MP_MANAGER_H__
