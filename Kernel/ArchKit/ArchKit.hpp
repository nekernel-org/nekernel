/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>

#ifdef __NEWOS_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <HALKit/AMD64/Hypervisor.hpp>
#include <HALKit/AMD64/Processor.hpp>
#elif defined(__NEWOS_PPC__)
#include <HALKit/POWER/Processor.hpp>
#elif defined(__NEWOS_ARM64__)
#include <HALKit/ARM64/Processor.hxx>
#else
#error !!! unknown architecture !!!
#endif

namespace NewOS
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
} // namespace NewOS

#define kKernelMaxSystemCalls (256)

typedef NewOS::Void (*rt_syscall_proc)(NewOS::VoidPtr);

struct RTSyscallInfoHdr final
{
	NewOS::Int64	fHash;
	NewOS::Bool		fHooked;
	rt_syscall_proc fProc;
};

inline NewOS::Array<RTSyscallInfoHdr,
					kKernelMaxSystemCalls>
	kSyscalls;

inline NewOS::Array<RTSyscallInfoHdr,
					kKernelMaxSystemCalls>
	kKerncalls;

EXTERN_C NewOS::HAL::StackFramePtr rt_get_current_context();
EXTERN_C NewOS::Void rt_do_context_switch(NewOS::HAL::StackFramePtr stackFrame);

#include <FirmwareKit/Handover.hxx>
