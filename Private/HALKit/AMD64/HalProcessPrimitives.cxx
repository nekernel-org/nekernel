/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/ProcessManager.hpp>

using namespace HCore;

Void Process::AssignStart(UIntPtr &imageStart) noexcept {
  if (imageStart == 0) this->Crash();

  this->StackFrame->Rbp = imageStart;
  this->StackFrame->Rsp = this->StackFrame->Rbp;
}
