/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Function.h>

#include <FirmwareKit/Handover.h>

#ifdef __NE_AMD64__
#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Hypervisor.h>
#include <HALKit/AMD64/Processor.h>
#elif defined(__NE_POWER64__)
#include <HALKit/POWER/Processor.h>
#elif defined(__NE_ARM64__)
#include <HALKit/ARM64/Processor.h>
#elif defined(__NE_SDK__)
#include <HALKit/ARM64/Processor.h>
#else
#error !!! unknown architecture !!!
#endif

#define kKernelMaxSystemCalls (512U)

namespace NeOS
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
	template <typename DataKind>
	inline Void ke_dma_write(UIntPtr base, DataKind reg, DataKind value) noexcept
	{
		*(volatile DataKind*)(base + reg) = value;
	}

	/// @brief read from mapped memory register.
	/// @param base base address
	/// @param reg the register.
	/// @return the value inside the register.
	template <typename DataKind>
	inline UInt32 ke_dma_read(UIntPtr base, DataKind reg) noexcept
	{
		return *(volatile DataKind*)(base + reg);
	}

	namespace HAL
	{
		auto mm_is_bitmap(VoidPtr ptr) -> Bool;
	}
} // namespace NeOS

typedef NeOS::Void (*rt_syscall_proc)(NeOS::VoidPtr);

struct HalSyscallEntry final
{
	NeOS::Int64		fHash;
	NeOS::Bool		fHooked;
	rt_syscall_proc fProc;

	operator bool()
	{
		return fHooked;
	}
};

inline NeOS::Array<HalSyscallEntry,
				   kKernelMaxSystemCalls>
	kSyscalls;

inline NeOS::Array<HalSyscallEntry,
				   kKernelMaxSystemCalls>
	kKerncalls;

EXTERN_C NeOS::HAL::StackFramePtr mp_get_current_context(NeOS::Int64 pid);
