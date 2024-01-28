/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "NewKit/Defines.hpp"
#include <KernelKit/ProcessManager.hpp>

using namespace hCore;

Void Process::AssignStart(UIntPtr &imageStart) noexcept
{
    if (imageStart == 0)
        this->Crash();

#ifdef __x86_64__
    this->StackFrame->Rbp = imageStart;
#elif defined(__powerpc)
    // link return register towards the __start symbol.
    this->StackFrame->R3 = imageStart;
#endif
}
