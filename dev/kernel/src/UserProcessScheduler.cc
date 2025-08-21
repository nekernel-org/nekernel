/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  FILE: UserProcessScheduler.cc
  PURPOSE: Low-Privilege/Ring-3 process scheduler.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cc
/// @brief Unprivileged/Ring-3 process scheduler.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/***********************************************************************************/

#include <ArchKit/ArchKit.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/HeapMgr.h>
#include <KernelKit/IPEFDylibObject.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/KString.h>
#include <NeKit/Utils.h>
#include <SignalKit/Signals.h>

///! BUGS: 0

namespace Kernel {
USER_PROCESS::USER_PROCESS()  = default;
USER_PROCESS::~USER_PROCESS() = default;

/// @brief Gets the last exit code.
/// @note Not thread-safe.
/// @return Int32 the last exit code.

/***********************************************************************************/
/// @brief Crashes the current process.
/***********************************************************************************/

Void USER_PROCESS::Crash() {
  if (this->Status != ProcessStatusKind::kRunning) return;

  this->Status = ProcessStatusKind::kKilled;

  (Void)(kout << this->Name << ": crashed, error id: " << number(-kErrorProcessFault) << kendl);
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

KPCError& USER_PROCESS::GetExitCode() noexcept {
  return this->LastExitCode;
}

/***********************************************************************************/
/// @brief Error code variable getter.
/***********************************************************************************/

KPCError& USER_PROCESS::GetLocalCode() noexcept {
  return this->LocalCode;
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
/** @param tree The tree to calibrate */
/***********************************************************************************/

template <typename T>
STATIC T* sched_try_go_upper_ptr_tree(T* tree) {
  if (!tree) {
    return nullptr;
  }

  tree = tree->Parent;

  if (tree) {
    auto tree_tmp = tree->Next;

    if (!tree_tmp) {
      return tree;
    }

    return tree_tmp;
  }

  return tree;
}

/***********************************************************************************/
/** @brief Allocate pointer to heap/file tree. */
/***********************************************************************************/

ErrorOr<VoidPtr> USER_PROCESS::New(SizeT sz, SizeT pad_amount) {
  if (this->UsedMemory > kSchedMaxMemoryLimit) return ErrorOr<VoidPtr>(-kErrorHeapOutOfMemory);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  auto vm_register = kKernelVM;

  hal_write_cr3(this->VMRegister);

  auto ptr = mm_alloc_ptr(sz, Yes, Yes, pad_amount);

  hal_write_cr3(vm_register);
#else
  auto ptr           = mm_alloc_ptr(sz, Yes, Yes, pad_amount);
#endif

  if (!this->HeapTree) {
    this->HeapTree = new PROCESS_HEAP_TREE<VoidPtr>();

    if (!this->HeapTree) {
      this->Crash();
      return ErrorOr<VoidPtr>(-kErrorHeapOutOfMemory);
    }

    this->HeapTree->EntryPad  = pad_amount;
    this->HeapTree->EntrySize = sz;

    this->HeapTree->Entry = ptr;

    this->HeapTree->Color = kBlackTreeKind;

    this->HeapTree->Prev   = nullptr;
    this->HeapTree->Next   = nullptr;
    this->HeapTree->Parent = nullptr;
    this->HeapTree->Child  = nullptr;
  } else {
    PROCESS_HEAP_TREE<VoidPtr>* entry      = this->HeapTree;
    PROCESS_HEAP_TREE<VoidPtr>* prev_entry = entry;

    BOOL is_parent = NO;

    while (entry) {
      if (entry->EntrySize < 1) break;

      prev_entry = entry;

      if (entry->Child && entry->Child->EntrySize > 0 && entry->Child->EntrySize == sz) {
        entry     = entry->Child;
        is_parent = YES;
      } else if (entry->Next && entry->Next->EntrySize > 0 && entry->Next->EntrySize == sz) {
        is_parent = NO;
        entry     = entry->Next;
      } else {
        entry = sched_try_go_upper_ptr_tree(entry);
        if (entry && entry->Color == kBlackTreeKind) break;
      }
    }

    auto new_entry = new PROCESS_HEAP_TREE<VoidPtr>();

    if (!new_entry) {
      this->Crash();
      return ErrorOr<VoidPtr>(-kErrorHeapOutOfMemory);
    }

    new_entry->Entry     = ptr;
    new_entry->EntrySize = sz;
    new_entry->EntryPad  = pad_amount;
    new_entry->Parent    = entry;
    new_entry->Child     = nullptr;
    new_entry->Next      = nullptr;
    new_entry->Prev      = nullptr;

    new_entry->Color  = kBlackTreeKind;
    prev_entry->Color = kRedTreeKind;

    if (is_parent) {
      prev_entry->Child = new_entry;
      new_entry->Parent = prev_entry;
    } else {
      prev_entry->Next = new_entry;
      new_entry->Prev  = prev_entry;
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

template <typename T>
STATIC Void sched_free_ptr_tree(T* tree) {
  // Deleting memory lists. Make sure to free all of them.
  while (tree) {
    if (tree->Entry) {
      MUST_PASS(mm_free_ptr(tree->Entry));
    }

    auto next = tree->Next;

    if (next->Child) sched_free_ptr_tree(next->Child);

    tree->Child = nullptr;

    mm_free_ptr(tree);

    tree = nullptr;
    tree = next;
  }
}

/***********************************************************************************/
/**
@brief Exit process method.
@param exit_code The process's exit code.
*/
/***********************************************************************************/

Void USER_PROCESS::Exit(const Int32& exit_code) {
  this->Status       = exit_code > 0 ? ProcessStatusKind::kKilled : ProcessStatusKind::kFrozen;
  this->LastExitCode = exit_code;
  this->UTime        = 0;

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  auto pd = kKernelVM;
  hal_write_cr3(this->VMRegister);
#endif

  sched_free_ptr_tree(this->HeapTree);
  this->HeapTree = nullptr;

  sched_free_ptr_tree(this->FileTree);
  this->FileTree = nullptr;

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  hal_write_cr3(pd);
#endif

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  //! Free the memory's page directory.
  if (this->VMRegister) HAL::mm_free_bitmap(this->VMRegister);
#endif

  //! Delete image if not done already.
  if (this->Image.fCode && mm_is_valid_ptr(this->Image.fCode)) mm_free_ptr(this->Image.fCode);

  //! Delete blob too.
  if (this->Image.fBlob && mm_is_valid_ptr(this->Image.fBlob)) mm_free_ptr(this->Image.fBlob);

  //! Delete stack frame.
  if (this->StackFrame && mm_is_valid_ptr(this->StackFrame))
    mm_free_ptr((VoidPtr) this->StackFrame);

  //! Avoid use after free.
  this->Image.fBlob = nullptr;
  this->Image.fCode = nullptr;
  this->StackFrame  = nullptr;

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

  --this->ParentTeam->mProcessCur;
}

/***********************************************************************************/
/// @brief Add dylib to the process object.
/***********************************************************************************/

Bool USER_PROCESS::InitDylib() {
  // React according to the process's kind.
  switch (this->Kind) {
    case USER_PROCESS::kExecutableDylibKind: {
      this->DylibDelegate = rtl_init_dylib_pef(*this);

      if (!this->DylibDelegate) {
        this->Crash();
        return NO;
      }

      return YES;
    }
    case USER_PROCESS::kExecutableKind: {
      return NO;
    }
    default: {
      break;
    }
  }

  (Void)(kout << "Unknown process kind: " << hex_number(this->Kind) << kendl);
  this->Crash();

  return NO;
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

  ProcessID pid = this->mTeam.mProcessCur;

  if (pid > kSchedProcessLimitPerTeam) {
    return -kErrorProcessFault;
  }

  ++this->mTeam.mProcessCur;

  USER_PROCESS& process = this->mTeam.mProcessList[pid];

  process.Image.fCode = code;
  process.Image.fBlob = image;

  SizeT len = rt_string_len(name);

  if (len > kSchedNameLen) {
    return -kErrorProcessFault;
  }

  rt_copy_memory_safe(reinterpret_cast<VoidPtr>(const_cast<Char*>(name)), process.Name, len,
                      kSchedNameLen);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  process.VMRegister = kKernelVM;
#else
  process.VMRegister = 0UL;
#endif  // ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

  process.StackFrame = new HAL::StackFrame();

  if (!process.StackFrame) {
    process.Crash();
    return -kErrorProcessFault;
  }

  rt_set_memory(process.StackFrame, 0, sizeof(HAL::StackFrame));

  process.StackFrame->IP = reinterpret_cast<UIntPtr>(code);
  process.StackFrame->SP = reinterpret_cast<UIntPtr>(&process.StackReserve[0] + process.StackSize);

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__
  HAL::mm_map_page((VoidPtr) process.StackFrame->IP,
                   (VoidPtr) HAL::mm_get_page_addr((VoidPtr) process.StackFrame->IP),
                   HAL::kMMFlagsUser | HAL::kMMFlagsPresent);
  HAL::mm_map_page((VoidPtr) process.StackFrame->SP,
                   (VoidPtr) HAL::mm_get_page_addr((VoidPtr) process.StackFrame->SP),
                   HAL::kMMFlagsUser | HAL::kMMFlagsPresent);
#endif  // ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

  process.StackSize = kSchedMaxStackSz;

  rt_set_memory(process.StackReserve, 0, process.StackSize);

  process.ParentTeam = &mTeam;

  process.ProcessId = pid;
  process.Status    = ProcessStatusKind::kRunning;
  process.PTime     = 0;
  process.RTime     = 0;

  if (!process.FileTree) {
    process.FileTree = new PROCESS_FILE_TREE<VoidPtr>();

    if (!process.FileTree) {
      process.Crash();
      return ErrorOr<VoidPtr>(-kErrorHeapOutOfMemory);
    }

    /// @todo File Tree allocation and dispose methods (amlal)
  }

  (Void)(kout << "PID: " << number(process.ProcessId) << kendl);
  (Void)(kout << "Name: " << process.Name << kendl);

  return pid;
}

/***********************************************************************************/
/// @brief Retrieves the singleton of the process scheduler.
/***********************************************************************************/

UserProcessScheduler& UserProcessScheduler::The() {
  STATIC UserProcessScheduler kScheduler;
  return kScheduler;
}

/***********************************************************************************/

/// @brief Remove process from the team.
/// @param process_id process slot inside team.
/// @retval true process was removed.
/// @retval false process doesn't exist in team.

/***********************************************************************************/

Void UserProcessScheduler::Remove(ProcessID process_id) {
  if (process_id < 0 || process_id > kSchedProcessLimitPerTeam) {
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
  if (mTeam.mProcessCur < 1) {
    return 0UL;
  }

  SizeT process_index = 0UL;  //! we store this guy to tell the scheduler how many
                              //! things we have scheduled.

  for (; process_index < mTeam.AsArray().Capacity(); ++process_index) {
    auto& process = mTeam.AsArray()[process_index];

    //! Check if the process needs to be run.
    if (UserProcessHelper::CanBeScheduled(process)) {
      kout << process.Name << " will be run...\r";

      //! Increase the usage time of the process.
      if (process.UTime < process.PTime) {
        ++process.UTime;
      }

      this->TheCurrentProcess() = process;

      if (UserProcessHelper::Switch(process.StackFrame, process.ProcessId)) {
        process.PTime = static_cast<Int32>(process.Affinity);

        // We add a bigger cooldown according to the RTime and affinity here.
        if (process.PTime < process.RTime && AffinityKind::kRealTime != process.Affinity) {
          if (process.RTime < (Int32) AffinityKind::kVeryHigh)
            process.RTime += (Int32) AffinityKind::kLowUsage;
          else if (process.RTime < (Int32) AffinityKind::kHigh)
            process.RTime += (Int32) AffinityKind::kStandard;
          else if (process.RTime < (Int32) AffinityKind::kStandard)
            process.RTime += (Int32) AffinityKind::kHigh;

          process.PTime -= process.RTime;
          process.RTime = 0UL;
        }
      }
    } else {
      ++process.RTime;
      --process.PTime;
    }
  }

  return process_index;
}

/// @brief Gets the current scheduled team.
/// @return
UserProcessTeam& UserProcessScheduler::TheCurrentTeam() {
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

  this->mTeam = team;

  return Yes;
}

/// @brief Gets current running process.
/// @return
Ref<USER_PROCESS>& UserProcessScheduler::TheCurrentProcess() {
  return mTeam.AsRef();
}

/// @brief Current proccess id getter.
/// @return USER_PROCESS ID integer.
ErrorOr<PID> UserProcessHelper::TheCurrentPID() {
  if (!UserProcessScheduler::The().TheCurrentProcess()) return ErrorOr<PID>{-kErrorProcessFault};

  kout << "UserProcessHelper::TheCurrentPID: Leaking ProcessId...\r";
  return ErrorOr<PID>{UserProcessScheduler::The().TheCurrentProcess().Leak().ProcessId};
}

/// @brief Check if process can be schedulded.
/// @param process the process reference.
/// @retval true can be schedulded.
/// @retval false cannot be schedulded.
Bool UserProcessHelper::CanBeScheduled(const USER_PROCESS& process) {
  if (process.Status != ProcessStatusKind::kRunning) return No;
  if (process.StackSize > kSchedMaxStackSz) return No;
  if (!process.Name[0]) return No;

  // real time processes shouldn't wait that much.
  if (process.Affinity == AffinityKind::kRealTime) return Yes;

  if (process.Signal.SignalID == SIGTRAP) {
    return No;
  }

  return process.PTime < 1;
}

/***********************************************************************************/
/**
 * @brief Start scheduling the current team.
 */
/***********************************************************************************/

SizeT UserProcessHelper::StartScheduling() {
  return UserProcessScheduler::The().Run();
}

/***********************************************************************************/
/**
 * \brief Does a context switch in a CPU.
 * \param the_stack the stackframe of the running app.
 * \param new_pid the process's PID.
 */
/***********************************************************************************/

Bool UserProcessHelper::Switch(HAL::StackFramePtr frame_ptr, PID new_pid) {
  (Void)(kout << "IP: " << hex_number(frame_ptr->IP) << kendl);

  for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Capacity(); ++index) {
    if (!HardwareThreadScheduler::The()[index].Leak()) continue;

    if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kAPInvalid ||
        HardwareThreadScheduler::The()[index].Leak()->Kind() == kAPBoot)
      continue;

    (Void)(kout << "AP_" << hex_number(index) << kendl);

    if (HardwareThreadScheduler::The()[index].Leak()->IsBusy()) {
      (Void)(kout << "AP_" << hex_number(index));
      kout << " is busy\r";

      continue;
    }

    (Void)(kout << "AP_" << hex_number(index));
    kout << " is now trying to run a new task!\r";

    ////////////////////////////////////////////////////////////
    ///	Prepare task switch.								 ///
    ////////////////////////////////////////////////////////////

    HardwareThreadScheduler::The()[index].Leak()->Busy(YES);

    Bool ret = HardwareThreadScheduler::The()[index].Leak()->Switch(frame_ptr);

    ////////////////////////////////////////////////////////////
    ///	Rollback on fail.    								 ///
    ////////////////////////////////////////////////////////////

    if (!ret) continue;

    UserProcessHelper::TheCurrentPID().Leak().Leak() = new_pid;

    HardwareThreadScheduler::The()[index].Leak()->fPTime =
        UserProcessScheduler::The().TheCurrentTeam().AsArray()[new_pid].PTime;

    (Void)(kout << "AP_" << hex_number(index));
    kout << " is now running a new task!\r";

    return YES;
  }

  kout << "Couldn't find a suitable core for the current process!\r";

  return NO;
}

////////////////////////////////////////////////////////////
/// @brief this checks if any process is on the team.
////////////////////////////////////////////////////////////
UserProcessScheduler::operator bool() {
  for (auto process_index = 0UL; process_index < mTeam.AsArray().Count(); ++process_index) {
    auto& process = mTeam.AsArray()[process_index];
    if (UserProcessHelper::CanBeScheduled(process)) return true;
  }

  return false;
}

////////////////////////////////////////////////////////////
/// @brief this checks if no process is on the team.
////////////////////////////////////////////////////////////
Bool UserProcessScheduler::operator!() {
  for (auto process_index = 0UL; process_index < mTeam.AsArray().Count(); ++process_index) {
    auto& process = mTeam.AsArray()[process_index];
    if (UserProcessHelper::CanBeScheduled(process)) return false;
  }

  return true;
}
}  // namespace Kernel
