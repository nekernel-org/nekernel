/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <FirmwareKit/Handover.hxx>

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
		Reg R8{0};
		Reg R9{0};
		Reg R10{0};
		Reg R11{0};
		Reg R12{0};
		Reg R13{0};
		Reg R14{0};
		Reg R15{0};
	};

	typedef StackFrame* StackFramePtr;
} // namespace Kernel::HAL

inline Kernel::VoidPtr kKernelVirtualStart = nullptr;
inline Kernel::UIntPtr kKernelVirtualSize  = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;

#include <HALKit/ARM64/HalPageAlloc.hxx>
