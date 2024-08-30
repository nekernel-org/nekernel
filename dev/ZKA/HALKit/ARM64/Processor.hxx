/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <FirmwareKit/Handover.hxx>

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif // ifdef kCPUBackendName

#define kPTESize 512 /* 64-bit PT */

#define kCPUBackendName "ARMv8"

namespace Kernel::HAL
{
	struct PACKED Register64 final
	{
		UShort	Limit;
		UIntPtr Base;
	};

	typedef UIntPtr	   Reg;
	typedef Register64 Register;

	/// @note let's keep the same name as AMD64 HAL.
	struct PACKED StackFrame final
	{
		Reg IntNum, Exception;
		Reg A0, A2, BP, SP, A3, A4, A5, A6;
		Reg R8, R9, R10, R11, R12, R13, R14, R15;
		Reg Gs, Fs;
	};

	typedef StackFrame* StackFramePtr;
} // namespace Kernel::HAL


inline Kernel::VoidPtr kKernelVMHStart	   = nullptr;
inline Kernel::VoidPtr kKernelVirtualStart = nullptr;
inline Kernel::UIntPtr kKernelVirtualSize  = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;

#include <HALKit/ARM64/HalPageAlloc.hxx>
