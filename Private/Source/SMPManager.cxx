/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/SMPManager.hpp>

///! BUGS: 0

///! @brief This file handles multi processing in NewOS.
///! @brief Multi processing is needed for multi-tasking operations.

namespace NewOS {
///! A HardwareThread class takes care of it's owned hardware thread.
///! It has a stack for it's core.

///! @brief constructor
HardwareThread::HardwareThread() = default;

///! @brief destructor
HardwareThread::~HardwareThread() = default;

//! @brief returns the id

const ThreadID& HardwareThread::ID() noexcept { return m_ID; }

//! @brief returns the kind

const ThreadKind& HardwareThread::Kind() noexcept { return m_Kind; }

//! @brief is the core busy?

bool HardwareThread::IsBusy() noexcept { return m_Busy; }

/// @brief Get processor stack frame.

HAL::StackFrame* HardwareThread::StackFrame() noexcept {
  MUST_PASS(m_Stack);
  return m_Stack;
}

void HardwareThread::Busy(const bool busy) noexcept { m_Busy = busy; }

HardwareThread::operator bool() { return m_Stack; }

/// @brief Wakeup the processor.

void HardwareThread::Wake(const bool wakeup) noexcept {
  m_Wakeup = wakeup;

  if (!m_Wakeup)
    rt_hang_thread(m_Stack);
  else
    rt_wakeup_thread(m_Stack);
}

extern bool rt_check_stack(HAL::StackFramePtr stackPtr);

bool HardwareThread::Switch(HAL::StackFrame* stack) {
  if (!rt_check_stack(stack)) return false;

  m_Stack = stack;

  rt_do_context_switch(m_Stack);
  return true;
}

///! @brief Tells if processor is waked up.
bool HardwareThread::IsWakeup() noexcept { return m_Wakeup; }

//! @brief Constructor and destructor

///! @brief Default constructor.
SMPManager::SMPManager() = default;

///! @brief Default destructor.
SMPManager::~SMPManager() = default;

/// @brief Shared singleton function
Ref<SMPManager> SMPManager::Shared() {
  static SMPManager manager;
  return {manager};
}

/// @brief Get Stack Frame of Core
HAL::StackFramePtr SMPManager::GetStackFrame() noexcept {
  if (m_ThreadList[m_CurrentThread].Leak() &&
      ProcessHelper::GetCurrentPID() ==
          m_ThreadList[m_CurrentThread].Leak().Leak().m_PID)
    return m_ThreadList[m_CurrentThread].Leak().Leak().m_Stack;

  return nullptr;
}

/// @brief Finds and switch to a free core.
bool SMPManager::Switch(HAL::StackFrame* stack) {
  if (stack == nullptr) return false;

  for (SizeT idx = 0; idx < kMaxHarts; ++idx) {
    // stack != nullptr -> if core is used, then continue.
    if (!m_ThreadList[idx].Leak() ||
        !m_ThreadList[idx].Leak().Leak().IsWakeup() ||
        m_ThreadList[idx].Leak().Leak().IsBusy())
      continue;

    // to avoid any null deref.
    if (!m_ThreadList[idx].Leak().Leak().m_Stack) continue;
    if (m_ThreadList[idx].Leak().Leak().m_Stack->Rsp == 0) continue;
    if (m_ThreadList[idx].Leak().Leak().m_Stack->Rbp == 0) continue;

    m_ThreadList[idx].Leak().Leak().Busy(true);

    m_ThreadList[idx].Leak().Leak().m_ID = idx;

    /// I figured out this:
    /// Allocate stack
    /// Set APIC base to stack
    /// Do stuff and relocate stack based on this code.
    /// - Amlel
    rt_copy_memory(stack, m_ThreadList[idx].Leak().Leak().m_Stack,
                   sizeof(HAL::StackFrame));

    m_ThreadList[idx].Leak().Leak().m_PID = ProcessHelper::GetCurrentPID();

    m_ThreadList[idx].Leak().Leak().Busy(false);

    return true;
  }

  return false;
}

/**
 * Index Hardware thread
 * @param idx the index
 * @return the reference to the hardware thread.
 */
Ref<HardwareThread> SMPManager::operator[](const SizeT& idx) {
  if (idx == 0) {
    if (m_ThreadList[idx].Leak().Leak().Kind() != kHartSystemReserved) {
      m_ThreadList[idx].Leak().Leak().m_Kind = kHartBoot;
    }
  } else if (idx >= kMaxHarts) {
    HardwareThread fakeThread;
    fakeThread.m_Kind = kInvalidHart;

    return {fakeThread};
  }

  return m_ThreadList[idx].Leak();
}

/**
 * Check if thread pool isn't empty.
 * @return
 */
SMPManager::operator bool() noexcept { return !m_ThreadList.Empty(); }

/**
 * Reverse operator bool
 * @return
 */
bool SMPManager::operator!() noexcept { return m_ThreadList.Empty(); }

/// @brief Returns the amount of core present.
/// @return the number of cores.
SizeT SMPManager::Count() noexcept { return m_ThreadList.Count(); }
}  // namespace NewOS
