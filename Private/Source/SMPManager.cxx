/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/SMPManager.hpp>

/// BUGS: 0

//! This file handles multi processing in hCore.
//! Multi processing is needed for File I/O, networking and scheduling.

namespace hCore {
// A ProcessorCore class takes care of it's owned hardware thread.
// It has a stack for it's core.

// @brief constructor
ProcessorCore::ProcessorCore() = default;

// @brief destructor
ProcessorCore::~ProcessorCore() = default;

//! @brief returns the id

const ThreadID& ProcessorCore::ID() noexcept { return m_ID; }

//! @brief returns the kind

const ThreadKind& ProcessorCore::Kind() noexcept { return m_Kind; }

//! @brief is the core busy?

bool ProcessorCore::IsBusy() noexcept { return m_Busy; }

/// @brief Get processor stack frame.

HAL::StackFrame* ProcessorCore::StackFrame() noexcept {
  MUST_PASS(m_Stack);
  return m_Stack;
}

void ProcessorCore::Busy(const bool busy) noexcept { m_Busy = busy; }

ProcessorCore::operator bool() { return m_Stack; }

/// @brief Wakeup the processor.

void ProcessorCore::Wake(const bool wakeup) noexcept {
  m_Wakeup = wakeup;

  if (!m_Wakeup)
    rt_hang_thread(m_Stack);
  else
    rt_wakeup_thread(m_Stack);
}

bool ProcessorCore::Switch(HAL::StackFrame* stack) {
  if (stack == nullptr) return false;

  return rt_do_context_switch(m_Stack, stack) == 0;
}

///! @brief Tells if processor is waked up.
bool ProcessorCore::IsWakeup() noexcept { return m_Wakeup; }

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

    m_ThreadList[idx].Leak().Leak().m_ID = idx;
    m_ThreadList[idx].Leak().Leak().m_Stack = stack;
    m_ThreadList[idx].Leak().Leak().m_PID = ProcessHelper::GetCurrentPID();

    m_ThreadList[idx].Leak().Leak().Busy(true);

    Boolean ret = (rt_do_context_switch(rt_get_current_context(), stack) == 0);

    m_ThreadList[idx].Leak().Leak().Busy(false);

    return ret;
  }

  return false;
}

/**
 * Index hart
 * @param idx
 * @return
 */
Ref<ProcessorCore> SMPManager::operator[](const SizeT& idx) {
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
}  // namespace hCore
