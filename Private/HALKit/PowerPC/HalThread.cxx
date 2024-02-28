/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <HALKit/PowerPC/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

extern "C" void flush_tlb() {}
extern "C" void rt_wait_400ns() {}

extern "C" HCore::HAL::StackFramePtr rt_get_current_context() { return nullptr; }
