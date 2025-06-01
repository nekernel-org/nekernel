/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <CFKit/Property.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/ProcessScheduler.h>

/***********************************************************************************/
///! @file HardwareThreadScheduler.cc
///! @brief This file handles multi processing in the Kernel.
///! @brief Multi processing is needed for multi-tasking operations.
/***********************************************************************************/

namespace Kernel {
/***********************************************************************************/
/// @note Those symbols are needed in order to switch and validate the stack.
/***********************************************************************************/

EXTERN_C Bool hal_check_task(HAL::StackFramePtr frame);
EXTERN_C Bool mp_register_task(HAL::StackFramePtr frame, ProcessID pid);

STATIC HardwareThreadScheduler kHardwareThreadScheduler;

///! A HardwareThread class takes care of it's owned hardware thread.
///! It has a stack for it's core.

/***********************************************************************************/
///! @brief C++ constructor.
/***********************************************************************************/
HardwareThread::HardwareThread() = default;

/***********************************************************************************/
///! @brief C++ destructor.
/***********************************************************************************/
HardwareThread::~HardwareThread() = default;

/***********************************************************************************/
//! @brief returns the id of the thread.
/***********************************************************************************/
ThreadID& HardwareThread::ID() noexcept {
  return fID;
}

/***********************************************************************************/
//! @brief returns the kind of thread we have.
/***********************************************************************************/
ThreadKind& HardwareThread::Kind() noexcept {
  return fKind;
}

/***********************************************************************************/
//! @brief is the thread busy?
//! @return whether the thread is busy or not.
/***********************************************************************************/
Bool HardwareThread::IsBusy() noexcept {
  return fBusy;
}

/***********************************************************************************/
/// @brief Get processor stack frame.
/***********************************************************************************/

HAL::StackFramePtr HardwareThread::StackFrame() noexcept {
  MUST_PASS(this->fStack);
  return this->fStack;
}

Void HardwareThread::Busy(Bool busy) noexcept {
  this->fBusy = busy;
}

HardwareThread::operator bool() {
  return this->fStack && !this->fBusy;
}

/***********************************************************************************/
/// @brief Wakeup the processor.
/***********************************************************************************/

Void HardwareThread::Wake(const bool wakeup) noexcept {
  this->fWakeup = wakeup;
}

/***********************************************************************************/
/// @brief Switch to hardware thread.
/// @param stack the new hardware thread.
/// @retval true stack was changed, code is running.
/// @retval false stack is invalid, previous code is running.
/***********************************************************************************/
Bool HardwareThread::Switch(HAL::StackFramePtr frame) {
  if (!frame) {
    return NO;
  }

  if (!hal_check_task(frame)) {
    return NO;
  }

  this->fStack = frame;
  return mp_register_task(fStack, this->fID);
}

/***********************************************************************************/
///! @brief Tells if processor is waked up.
/***********************************************************************************/
bool HardwareThread::IsWakeup() noexcept {
  return this->fWakeup;
}

/***********************************************************************************/
///! @brief Constructor and destructors.
///! @brief Default constructor.
/***********************************************************************************/

HardwareThreadScheduler::HardwareThreadScheduler() = default;

/***********************************************************************************/
///! @brief Default destructor.
/***********************************************************************************/
HardwareThreadScheduler::~HardwareThreadScheduler() = default;

/***********************************************************************************/
/// @brief Shared singleton function
/***********************************************************************************/
HardwareThreadScheduler& HardwareThreadScheduler::The() {
  return kHardwareThreadScheduler;
}

/***********************************************************************************/
/// @brief Get Stack Frame of AP.
/***********************************************************************************/
HAL::StackFramePtr HardwareThreadScheduler::Leak() noexcept {
  return fThreadList[fCurrentThread].fStack;
}

/***********************************************************************************/
/**
 * Get Hardware thread at index.
 * @param idx the index
 * @return the reference to the hardware thread.
 */
/***********************************************************************************/
Ref<HardwareThread*> HardwareThreadScheduler::operator[](SizeT idx) {
  if (idx > kMaxAPInsideSched) {
    HardwareThread* kFakeThread = nullptr;
    return {kFakeThread};
  }

  return &fThreadList[idx];
}

/***********************************************************************************/
/**
 * Check if thread pool isn't empty.
 * @return
 */
/***********************************************************************************/
HardwareThreadScheduler::operator bool() noexcept {
  return !fThreadList.Empty();
}

/***********************************************************************************/
/**
 * Reverse operator bool
 * @return
 */
/***********************************************************************************/
bool HardwareThreadScheduler::operator!() noexcept {
  return fThreadList.Empty();
}

/***********************************************************************************/
/// @brief Returns the amount of core present.
/// @return the number of APs.
/***********************************************************************************/
SizeT HardwareThreadScheduler::Capacity() noexcept {
  return fThreadList.Count();
}
}  // namespace Kernel
