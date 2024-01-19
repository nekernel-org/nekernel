/*
*	========================================================
*
*	hCore
* 	Copyright Mahrouss Logic, all rights reserved.
*
* 	========================================================
*/

#include <HALKit/PowerPC/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

extern "C" void flush_tlb() {}
extern "C" void rt_wait_for_io() {}
extern "C" hCore::HAL::StackFrame* rt_get_current_context() { }

namespace hCore
{
namespace HAL
{
    UIntPtr hal_create_page(bool rw, bool user)
    {
        return 0;
    }

    UIntPtr hal_alloc_page(UIntPtr offset, bool rw, bool user)
    {
        return 0;
    }
}

// @brief wakes up thread.
// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack)
{

}

// @brief makes thread sleep.
// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack)
{

}

// @brief main HAL entrypoint
void initialize_hardware_components()
{

}

void system_io_print(const char *bytes)
{
    if (!bytes)
        return;

    SizeT index = 0;
    SizeT len = string_length(bytes, 256);

    while (index < len)
    {
        // TODO
        ++index;
    }
}

TerminalDevice kcout(hCore::system_io_print, nullptr);
}