/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <HALKit/PowerPC/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

using namespace NewOS;

/// @brief Flush system TLB, looks like the PowerPC version, as it acts the same, no specific instruction for that.
/// @note The 88K MMU should be present in the die.
EXTERN_C void hal_flush_tlb() { asm volatile("isync;invltlb;msync;isync"); }
