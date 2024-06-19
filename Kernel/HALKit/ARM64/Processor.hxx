/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>
#include <FirmwareKit/Handover.hxx>

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif // ifdef kCPUBackendName

#define kPTESize 512 /* 64-bit PT */

#define kCPUBackendName "ARM64"

#ifdef __ZETA_MACHINE__
#define kVirtualAddressStartOffset (0x80000000)
#else
#error !!! please provide that macro. !!!
#endif

namespace NewOS::HAL
{
    struct PACKED Register64 final
	{
		UShort	Limit;
		UIntPtr Base;
	};

	typedef UIntPtr Reg;
	typedef Register64 Register;

	struct PACKED StackFrame final
	{
		Reg IntNum, Exception;
		Reg Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
		Reg R8, R9, R10, R11, R12, R13, R14, R15;
		Reg Gs, Fs;
	};

	typedef StackFrame* StackFramePtr;
}

inline NewOS::VoidPtr kKernelVirtualStart = (NewOS::VoidPtr)kVirtualAddressStartOffset;
inline NewOS::UIntPtr kKernelVirtualSize  = 0UL;

inline NewOS::VoidPtr kKernelPhysicalStart = nullptr;

#include <HALKit/ARM64/HalPageAlloc.hpp>
