/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  FILE: UserProcessScheduler.cc
  PURPOSE: Low level/Ring-3 process scheduler.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cc
/// @brief Unprivileged/Ring-3 process scheduler.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/***********************************************************************************/

#include <ArchKit/ArchKit.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/IPEFDylibObject.h>
#include <KernelKit/KPC.h>
#include <KernelKit/MemoryMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NewKit/KString.h>
#include "KernelKit/CoreProcessScheduler.h"
#include "NewKit/Defines.h"

///! BUGS: 0

namespace Kernel {
/***********************************************************************************/
/// @brief Exit Code global variable.
/***********************************************************************************/

STATIC UInt32 kLastExitCode = 0U;

STATIC BOOL kCurrentlySwitching = No;

/***********************************************************************************/
/// @brief Scheduler itself.
/***********************************************************************************/

STATIC UserProcessScheduler kScheduler;

USER_PROCESS::USER_PROCESS()  = default;
USER_PROCESS::~USER_PROCESS() = default;

/// @brief Gets the last exit code.
/// @note Not thread-safe.
/// @return Int32 the last exit code.

const UInt32& sched_get_exit_code(void) noexcept {
  return kLastExitCode;
}

/***********************************************************************************/
/// @brief Crashes the current process.
/***********************************************************************************/

Void USER_PROCESS::Crash() {
  if (this->Status != ProcessStatusKind::kRunning) return;

  (Void)(kout << this->Name << ": crashed, error id: " << number(-kErrorProcessFault) << kendl);
  this->Exit(-kErrorProcessFault);
}

/***********************************************************************************/
/// @brief boolean operator, check status.
/***********************************************************************************/

USER_PROCESS::operator bool() {
  return this->Status == ProcessStatusKind::kRunning;
}

/***********************************************************************************/
/// @brief Gets the local last exit code.
/// @note Not thread-safe.
/// @return Int32 the last exit code.
/***********************************************************************************/

const UInt32& USER_PROCESS::GetExitCode() noexcept {
  return this->fLastExitCode;
}

/***********************************************************************************/
/// @brief Error code variable getter.
/***********************************************************************************/

Int32& USER_PROCESS::GetLocalCode() noexcept {
  return this->fLocalCode;
}

/***********************************************************************************/
/// @brief Wakes process header.
/// @param should_wakeup if the program shall wakeup or not.
/***********************************************************************************/

Void USER_PROCESS::Wake(Bool should_wakeup) {
  this->Status = should_wakeup ? ProcessStatusKind::kRunning : ProcessStatusKind::kFrozen;
}

/***********************************************************************************/
/** @brief Allocate pointer to heap tree. */
/***********************************************************************************/

STATIC USER_PROCESS::USER_HEAP_TREE* sched_try_go_upper_heap_tree(
    USER_PROCESS::USER_HEAP_TREE* tree) {
  if (!tree) {
    return nullptr;
  }

  tree = tree->MemoryParent;

  if (tree) {
    auto tree_tmp = tree->MemoryNext;

    if (!tree_tmp) {
      return tree;
    }

    return tree_tmp;
  }

  return tree;
}

/***********************************************************************************/
/** @brief Allocate pointer to heap tree. */
/***********************************************************************************/

ErrorOr<VoidPtr> USER_PROCESS::New(SizeT sz, SizeT pad_amount) {
  if (this->UsedMemory > kSchedMaxMemoryLimit) return ErrorOr<VoidPtr>(-kErrorHeapOutOfMemory);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  auto vm_register = hal_read_cr3();
  hal_write_cr3(this->VMRegister);

  auto ptr = mm_new_heap(sz, Yes, Yes, pad_amount);

  hal_write_cr3(vm_register);
#else
  auto ptr = mm_new_heap(sz, Yes, Yes, pad_amount);
#endif

  if (!this->HeapTree) {
    this->HeapTree = new USER_HEAP_TREE();

    this->HeapTree->MemoryColor = USER_HEAP_TREE::kBlackMemory;

    this->HeapTree->MemoryEntryPad  = pad_amount;
    this->HeapTree->MemoryEntrySize = sz;

    this->HeapTree->MemoryEntry = ptr;

    this->HeapTree->MemoryPrev   = nullptr;
    this->HeapTree->MemoryNext   = nullptr;
    this->HeapTree->MemoryParent = nullptr;
    this->HeapTree->MemoryChild  = nullptr;
  } else {
    USER_HEAP_TREE* entry      = this->HeapTree;
    USER_HEAP_TREE* prev_entry = entry;

    BOOL is_parent = NO;

    while (entry) {
      if (entry->MemoryEntrySize < 1) break;

      prev_entry = entry;

      if (entry->MemoryNext) {
        is_parent = NO;
        entry     = entry->MemoryNext;
      } else if (entry->MemoryChild) {
        entry     = entry->MemoryChild;
        is_parent = YES;
      } else {
        entry = sched_try_go_upper_heap_tree(entry);
      }
    }

    if (!entry) entry = new USER_HEAP_TREE();

    entry->MemoryEntry     = ptr;
    entry->MemoryEntrySize = sz;
    entry->MemoryEntryPad  = pad_amount;

    if (is_parent) {
      entry->MemoryParent     = prev_entry;
      prev_entry->MemoryChild = entry;

      prev_entry->MemoryColor = USER_HEAP_TREE::kBlackMemory;
      entry->MemoryColor      = USER_HEAP_TREE::kRedMemory;
    } else {
      prev_entry->MemoryNext = entry;
      entry->MemoryPrev      = prev_entry;
    }
  }

  this->UsedMemory += sz;

  return ErrorOr<VoidPtr>(ptr);
}

/***********************************************************************************/
/// @brief Gets the name of the current process.
/***********************************************************************************/

const Char* USER_PROCESS::GetName() noexcept {
  return this->Name;
}

/***********************************************************************************/
/// @brief Gets the owner of the process.
/***********************************************************************************/

const User* USER_PROCESS::GetOwner() noexcept {
  return this->Owner;
}

/// @brief USER_PROCESS status getter.
const ProcessStatusKind& USER_PROCESS::GetStatus() noexcept {
  return this->Status;
}

/***********************************************************************************/
/**
@brief Affinity is the time slot allowed for the process.
*/
/***********************************************************************************/

const AffinityKind& USER_PROCESS::GetAffinity() noexcept {
  return this->Affinity;
}

/***********************************************************************************/
/** @brief Free heap tree. */
/***********************************************************************************/

STATIC Void sched_free_heap_tree(USER_PROCESS::USER_HEAP_TREE* memory_heap_list) {
  // Deleting memory lists. Make sure to free all of them.
  while (memory_heap_list) {
    if (memory_heap_list->MemoryEntry) {
      MUST_PASS(mm_delete_heap(memory_heap_list->MemoryEntry));
    }

    auto next = memory_heap_list->MemoryNext;

    mm_delete_heap(memory_heap_list);

    if (memory_heap_list->MemoryChild) sched_free_heap_tree(memory_heap_list->MemoryChild);

    memory_heap_list = next;
  }
}

/***********************************************************************************/
/**
@brief Exit process method.
@param exit_code The process's exit code.
*/
/***********************************************************************************/

Void USER_PROCESS::Exit(const Int32& exit_code) {
  this->Status        = exit_code > 0 ? ProcessStatusKind::kKilled : ProcessStatusKind::kFrozen;
  this->fLastExitCode = exit_code;

  kLastExitCode = exit_code;

  auto memory_heap_list = this->HeapTree;

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  auto pd = hal_read_cr3();
  hal_write_cr3(this->VMRegister);
#endif

  sched_free_heap_tree(memory_heap_list);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  hal_write_cr3(pd);
#endif

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  //! Free the memory's page directory.
  if (this->VMRegister) HAL::mm_free_bitmap(this->VMRegister);
#endif

  //! Delete image if not done already.
  if (this->Image.fCode && mm_is_valid_heap(this->Image.fCode)) mm_delete_heap(this->Image.fCode);

  //! Delete blob too.
  if (this->Image.fBlob && mm_is_valid_heap(this->Image.fBlob)) mm_delete_heap(this->Image.fBlob);

  //! Delete stack frame.
  if (this->StackFrame && mm_is_valid_heap(this->StackFrame))
    mm_delete_heap((VoidPtr) this->StackFrame);

  //! Delete stack reserve.
  if (this->StackReserve && mm_is_valid_heap(this->StackReserve))
    mm_delete_heap(reinterpret_cast<VoidPtr>(this->StackReserve));

  //! Avoid use after free.
  this->Image.fBlob  = nullptr;
  this->Image.fCode  = nullptr;
  this->StackFrame   = nullptr;
  this->StackReserve = nullptr;

  if (this->Kind == kExecutableDylibKind) {
    Bool success = false;

    rtl_fini_dylib_pef(*this, reinterpret_cast<IPEFDylibObject*>(this->DylibDelegate), &success);

    if (!success) {
      ke_panic(RUNTIME_CHECK_PROCESS);
    }

    this->DylibDelegate = nullptr;
  }

  this->ProcessId = 0UL;
  this->Status    = ProcessStatusKind::kFinished;

  --this->ParentTeam->mProcessCount;
}

/***********************************************************************************/
/// @brief Add process to team.
/// @param process the process *Ref* class.
/// @return the process index inside the team.
/***********************************************************************************/

ProcessID UserProcessScheduler::Spawn(const Char* name, VoidPtr code, VoidPtr image) {
  if (!name || !code) {
    return -kErrorProcessFault;
  }

  if (*name == 0) {
    return -kErrorProcessFault;
  }

  ProcessID pid = this->mTeam.mProcessCount;

  if (pid > kSchedProcessLimitPerTeam) {
    return -kErrorProcessFault;
  }

  ++this->mTeam.mProcessCount;

  USER_PROCESS& process = this->mTeam.mProcessList[pid];

  process.Image.fCode = code;
  process.Image.fBlob = image;

  SizeT len = rt_string_len(name);

  if (len > kSchedNameLen) {
    return -kErrorProcessFault;
  }

  rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(name)), process.Name, len);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  process.VMRegister = new PDE();

  if (!process.VMRegister) {
    process.Crash();
    return -kErrorProcessFault;
  }

  UInt32 flags = HAL::kMMFlagsPresent;
  flags |= HAL::kMMFlagsWr;
  flags |= HAL::kMMFlagsUser;

  HAL::mm_map_page((VoidPtr) process.VMRegister, process.VMRegister, flags);
#endif  // ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

  process.StackFrame = new HAL::StackFrame();

  if (!process.StackFrame) {
    process.Crash();
    return -kErrorProcessFault;
  }

  rt_set_memory(process.StackFrame, 0, sizeof(HAL::StackFrame));

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  flags = HAL::kMMFlagsPresent;
  flags |= HAL::kMMFlagsWr;
  flags |= HAL::kMMFlagsUser;

  HAL::mm_map_page((VoidPtr) process.StackFrame, process.StackFrame, flags);
#endif  // ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

  // React according to process kind.
  switch (process.Kind) {
    case USER_PROCESS::kExecutableDylibKind: {
      process.DylibDelegate = rtl_init_dylib_pef(process);
      MUST_PASS(process.DylibDelegate);
      break;
    }
    case USER_PROCESS::kExecutableKind: {
      break;
    }
    default: {
      (Void)(kout << "Unknown process kind: " << hex_number(process.Kind) << kendl);
      break;
    }
  }

  process.StackReserve = new UInt8[process.StackSize];

  if (!process.StackReserve) {
    process.Crash();
    return -kErrorProcessFault;
  }

  rt_set_memory(process.StackReserve, 0, process.StackSize);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  flags = HAL::kMMFlagsPresent;
  flags |= HAL::kMMFlagsWr;
  flags |= HAL::kMMFlagsUser;

  HAL::mm_map_page((VoidPtr) process.StackReserve, process.StackReserve, flags);
#endif  // ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

  process.ParentTeam = &mTeam;

  process.ProcessId = pid;
  process.Status    = ProcessStatusKind::kStarting;
  process.PTime     = (UIntPtr) AffinityKind::kStandard;
  process.RTime     = 0;

  (Void)(kout << "PID: " << number(process.ProcessId) << kendl);
  (Void)(kout << "Name: " << process.Name << kendl);

  return pid;
}

/***********************************************************************************/
/// @brief Retrieves the singleton of the process scheduler.
/***********************************************************************************/

UserProcessScheduler& UserProcessScheduler::The() {
  return kScheduler;
}

/***********************************************************************************/

/// @brief Remove process from the team.
/// @param process_id process slot inside team.
/// @retval true process was removed.
/// @retval false process doesn't exist in team.

/***********************************************************************************/

Void UserProcessScheduler::Remove(ProcessID process_id) {
  if (process_id < 0 || process_id >= kSchedProcessLimitPerTeam) {
    return;
  }

  if (this->mTeam.mProcessList[process_id].Status == ProcessStatusKind::kInvalid) {
    return;
  }

  mTeam.mProcessList[process_id].Exit(0);
}

/// @brief Is it a user scheduler?

Bool UserProcessScheduler::IsUser() {
  return Yes;
}

/// @brief Is it a kernel scheduler?

Bool UserProcessScheduler::IsKernel() {
  return No;
}

/// @brief Is it a SMP scheduler?

Bool UserProcessScheduler::HasMP() {
  MUST_PASS(kHandoverHeader);
  return kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled;
}

/***********************************************************************************/
/// @brief Run User scheduler object.
/// @return USER_PROCESS count executed within a team.
/***********************************************************************************/

SizeT UserProcessScheduler::Run() noexcept {
  SizeT process_index = 0UL;  //! we store this guy to tell the scheduler how many
                              //! things we have scheduled.

  if (mTeam.mProcessCount < 1) {
    return 0UL;
  }

  kCurrentlySwitching = Yes;

  for (; process_index < mTeam.AsArray().Capacity(); ++process_index) {
    auto& process = mTeam.AsArray()[process_index];

    //! check if the process needs to be run.
    if (UserProcessHelper::CanBeScheduled(process)) {
      // Set current process header.
      this->CurrentProcess() = process;

      process.PTime = static_cast<Int32>(process.Affinity);

      // tell helper to find a core to schedule on.
      BOOL ret = UserProcessHelper::Switch(process.Image.Leak().Leak().Leak(),
                                           &process.StackReserve[process.StackSize - 1],
                                           process.StackFrame, process.ProcessId);

      if (!ret) {
        kout << "The process: " << process.Name << ", is not valid! Crashing it...\r";
        process.Crash();
      }
    } else {
      if (process.ProcessId == this->CurrentProcess().Leak().ProcessId) {
        if (process.PTime < process.RTime) {
          if (process.RTime < (Int32) AffinityKind::kRealTime)
            process.PTime = (Int32) AffinityKind::kVeryLowUsage;
          else if (process.RTime < (Int32) AffinityKind::kVeryHigh)
            process.PTime = (Int32) AffinityKind::kLowUsage;
          else if (process.RTime < (Int32) AffinityKind::kHigh)
            process.PTime = (Int32) AffinityKind::kStandard;
          else if (process.RTime < (Int32) AffinityKind::kStandard)
            process.PTime = (Int32) AffinityKind::kHigh;

          process.RTime = static_cast<Int32>(process.Affinity);

          BOOL ret = UserProcessHelper::Switch(process.Image.Leak().Leak().Leak(),
                                               &process.StackReserve[process.StackSize - 1],
                                               process.StackFrame, process.ProcessId);

          if (!ret) {
            kout << "The process: " << process.Name << ", is not valid! Crashing it...\r";
            process.Crash();
          }
        } else {
          ++process.RTime;
        }
      }

      --process.PTime;
    }
  }

  kCurrentlySwitching = No;

  return process_index;
}

/// @brief Gets the current scheduled team.
/// @return
UserProcessTeam& UserProcessScheduler::CurrentTeam() {
  return mTeam;
}

/***********************************************************************************/
/// @brief Switches the current team.
/// @param team the new team to switch to.
/// @retval true team was switched.
/// @retval false team was not switched.
/***********************************************************************************/

BOOL UserProcessScheduler::SwitchTeam(UserProcessTeam& team) {
  if (team.AsArray().Count() < 1) return No;

  if (kCurrentlySwitching) return No;

  kScheduler.mTeam = team;

  return Yes;
}

/// @brief Gets current running process.
/// @return
Ref<USER_PROCESS>& UserProcessScheduler::CurrentProcess() {
  return mTeam.AsRef();
}

/// @brief Current proccess id getter.
/// @return USER_PROCESS ID integer.
ErrorOr<PID> UserProcessHelper::TheCurrentPID() {
  if (!kScheduler.CurrentProcess()) return ErrorOr<PID>{-kErrorProcessFault};

  kout << "UserProcessHelper::TheCurrentPID: Leaking ProcessId...\r";
  return ErrorOr<PID>{kScheduler.CurrentProcess().Leak().ProcessId};
}

/// @brief Check if process can be schedulded.
/// @param process the process reference.
/// @retval true can be schedulded.
/// @retval false cannot be schedulded.
Bool UserProcessHelper::CanBeScheduled(const USER_PROCESS& process) {
  if (process.Status == ProcessStatusKind::kKilled ||
      process.Status == ProcessStatusKind::kFinished ||
      process.Status == ProcessStatusKind::kStarting ||
      process.Status == ProcessStatusKind::kFrozen)
    return No;

  if (process.Status == ProcessStatusKind::kInvalid) return No;

  if (!process.Image.HasCode()) return No;

  if (!process.Name[0]) return No;

  // real time processes shouldn't wait that much.
  if (process.Affinity == AffinityKind::kRealTime) return Yes;

  return process.PTime < 1;
}

/***********************************************************************************/
/**
 * @brief Start scheduling current AP.
 */
/***********************************************************************************/

SizeT UserProcessHelper::StartScheduling() {
  return kScheduler.Run();
}

/***********************************************************************************/
/**
 * \brief Does a context switch in a CPU.
 * \param the_stack the stackframe of the running app.
 * \param new_pid the process's PID.
 */
/***********************************************************************************/

Bool UserProcessHelper::Switch(VoidPtr image, UInt8* stack, HAL::StackFramePtr frame_ptr,
                               PID new_pid) {
  for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Capacity(); ++index) {
    if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kAPInvalid ||
        HardwareThreadScheduler::The()[index].Leak()->Kind() == kAPBoot)
      continue;

    // A fallback is a special core for real-time tasks which needs immediate execution.
    if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kAPRealTime) {
      if (UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid].Affinity !=
          AffinityKind::kRealTime)
        continue;

      if (HardwareThreadScheduler::The()[index].Leak()->Switch(image, stack, frame_ptr, new_pid)) {
        HardwareThreadScheduler::The()[index].Leak()->fPTime =
            UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid].PTime;

        UserProcessHelper::TheCurrentPID().Leak().Leak() = UserProcessHelper::TheCurrentPID();

        return YES;
      }

      continue;
    }

    if (UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid].Affinity ==
        AffinityKind::kRealTime)
      continue;

    ////////////////////////////////////////////////////////////
    ///	Prepare task switch.								 ///
    ////////////////////////////////////////////////////////////

    Bool ret =
        HardwareThreadScheduler::The()[index].Leak()->Switch(image, stack, frame_ptr, new_pid);

    ////////////////////////////////////////////////////////////
    ///	Rollback on fail.    								 ///
    ////////////////////////////////////////////////////////////

    if (!ret) continue;

    UserProcessHelper::TheCurrentPID().Leak().Leak() = new_pid;

    HardwareThreadScheduler::The()[index].Leak()->fPTime =
        UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid].PTime;
    HardwareThreadScheduler::The()[index].Leak()->Wake(YES);

    return Yes;
  }

  return No;
}

////////////////////////////////////////////////////////////
/// @brief this checks if any process is on the team.
////////////////////////////////////////////////////////////
UserProcessScheduler::operator bool() {
  return mTeam.AsArray().Count() > 0;
}

////////////////////////////////////////////////////////////
/// @brief this checks if no process is on the team.
////////////////////////////////////////////////////////////
Bool UserProcessScheduler::operator!() {
  return mTeam.AsArray().Count() == 0;
}
}  // namespace Kernel
