/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>

#ifdef __NEWOS_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hxx>
#include <HALKit/AMD64/Hypervisor.hpp>
#include <HALKit/AMD64/Processor.hxx>
#elif defined(__NEWOS_POWER64__)
#include <HALKit/POWER/Processor.hxx>
#elif defined(__NEWOS_ARM64__)
#include <HALKit/ARM64/Processor.hxx>
#else
#error !!! unknown architecture !!!
#endif

namespace Kernel
{
	constexpr static inline SSizeT rt_hash_seed(const char* seed, int mul)
	{
		SSizeT hash = 0;

		for (SSizeT idx = 0; seed[idx] != 0; ++idx)
		{
			hash += seed[idx];
			hash ^= mul;
		}

		return hash;
	}

	/// @brief write to mapped memory register
	/// @param base the base address.
	/// @param reg the register.
	/// @param value the write to write on it.
	inline void ke_dma_write(UInt32 base, UInt32 reg, UInt32 value) noexcept
	{
		*(volatile UInt32*)((UInt64)base + reg) = value;
	}

	/// @brief read from mapped memory register.
	/// @param base base address
	/// @param reg the register.
	/// @return the value inside the register.
	inline UInt32 ke_dma_read(UInt32 base, UInt32 reg) noexcept
	{
		return *(volatile UInt32*)((UInt64)base + reg);
	}

	/// @brief Print a region of memory.
	/// @param start
	/// @param length
	inline void ke_print_raw_memory(const void* start, Size length)
	{
		const UInt8* ptr = (const UInt8*)start;
		for (Size i = 0; i < length; i++)
		{
			if (i % 16 == 0)
			{
				kcout << hex_number((UIntPtr)ptr + i);
			}
			else
			{
				kcout << hex_number(ptr[i]);
			}

			kcout << " ";
		}

		kcout << "\r";
	}
} // namespace Kernel

#define kKernelMaxSystemCalls (256)

typedef Kernel::Void (*rt_syscall_proc)(Kernel::VoidPtr);

struct RTSyscallInfoHdr final
{
	Kernel::Int64	fHash;
	Kernel::Bool	fHooked;
	rt_syscall_proc fProc;
};

inline Kernel::Array<RTSyscallInfoHdr,
					 kKernelMaxSystemCalls>
	kSyscalls;

inline Kernel::Array<RTSyscallInfoHdr,
					 kKernelMaxSystemCalls>
	kKerncalls;

EXTERN_C Kernel::HAL::StackFramePtr rt_get_current_context();
EXTERN_C Kernel::Void rt_do_context_switch(Kernel::HAL::StackFramePtr stackFrame);

#include <FirmwareKit/Handover.hxx>
