/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

using namespace Kernel;

/// @brief Flush system TLB, looks like the POWER version, as it acts the same, no specific instruction for that.
/// @note The 88K MMU should be present as well.
EXTERN_C void hal_flush_tlb()
{
	asm volatile("invltlb");
}
