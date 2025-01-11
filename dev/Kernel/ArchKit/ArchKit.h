/* -------------------------------------------

	Copyright (C) 2024, t& Corporation, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>

#include <FirmwareKit/Handover.h>

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Hypervisor.h>
#include <HALKit/AMD64/Processor.h>
#elif defined(__ZKA_POWER64__)
#include <HALKit/POWER/Processor.h>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Processor.h>
#else
#error !!! unknown architecture !!!
#endif

namespace Kernel
{
	inline SSizeT rt_hash_seed(const Char* seed, int mul)
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
	template <typename WordLength>
	inline Void ke_dma_write(WordLength base, WordLength reg, WordLength value) noexcept
	{
		*(volatile WordLength*)(base + reg) = value;
	}

	/// @brief read from mapped memory register.
	/// @param base base address
	/// @param reg the register.
	/// @return the value inside the register.
	template <typename WordLength>
	inline UInt32 ke_dma_read(WordLength base, WordLength reg) noexcept
	{
		return *(volatile WordLength*)((UInt64)base + reg);
	}

	namespace HAL
	{
		auto mm_is_bitmap(VoidPtr ptr) -> Bool;
	}
} // namespace Kernel

#define kKernelMaxSystemCalls (256)

typedef Kernel::Void (*rt_syscall_proc)(Kernel::VoidPtr);

struct HAL_SYSCALL_RECORD final
{
	Kernel::Int64	fHash;
	Kernel::Bool	fHooked;
	rt_syscall_proc fProc;

	operator bool()
	{
		return fHooked;
	}
};

inline Kernel::Array<HAL_SYSCALL_RECORD,
					 kKernelMaxSystemCalls>
	kSyscalls;

inline Kernel::Array<HAL_SYSCALL_RECORD,
					 kKernelMaxSystemCalls>
	kKerncalls;

EXTERN_C Kernel::HAL::StackFramePtr mp_get_current_context(Kernel::Int64 pid);
