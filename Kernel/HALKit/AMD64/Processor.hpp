/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: Prcoessor.hxx
	Purpose: AMD64 processor abstraction.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>
#include <FirmwareKit/Handover.hxx>
#include <HALKit/AMD64/HalPageAlloc.hpp>

#include <cpuid.h>

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif // ifdef kCPUBackendName

#define kCPUBackendName "AMD64"

#define IsActiveLow(FLG)	  (FLG & 2)
#define IsLevelTriggered(FLG) (FLG & 8)

#define kInterruptGate	 (0x8E)
#define kTrapGate		 (0xEF)
#define kTaskGate		 (0b10001100)
#define kGdtCodeSelector (0x08)
#define cHeapStartOffset (0x10000000)

namespace Kernel
{
	namespace Detail::AMD64
	{
		struct PACKED InterruptDescriptorAMD64 final
		{
			UInt16 OffsetLow; // offset bits 0..15
			UInt16 Selector;  // a code segment selector in GDT or LDT
			UInt8
				   Ist;			   // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
			UInt8  TypeAttributes; // gate type, dpl, and p fields
			UInt16 OffsetMid;	   // offset bits 16..31
			UInt32 OffsetHigh;	   // offset bits 32..63
			UInt32 Zero;		   // reserved
		};
	} // namespace Detail::AMD64
} // namespace Kernel

namespace Kernel::HAL
{
	/// @brief Virtual memory flags.
	enum
	{
		eFlagsUser,
		eFlagsRw,
		eFlagsExecDisable
	};

	/// @brief Map address to PDE.
	/// @param pde a valid page directory.
	/// @param phys_addr a valid phyiscal address.
	/// @param virt_addr a valid virtual address.
	/// @param flags the flags to put on the page.
	inline Int32 ke_map_address(PDE* pde, UIntPtr phys_addr, UIntPtr virt_addr, UInt32 flags)
	{
		UInt16 pml4_index = (virt_addr >> 39) & 0x1FF;

		if (!pde->Pte[pml4_index].Present)
		{
			pde->Pte[pml4_index].Present = true;
			kcout << "PM is present now.\r";

			pde->Pte[pml4_index].PhysicalAddress = phys_addr;
			pde->Pte[pml4_index].Rw				 = flags & eFlagsRw;
			pde->Pte[pml4_index].User			 = flags & eFlagsUser;
			pde->Pte[pml4_index].ExecDisable	 = flags & eFlagsExecDisable;

			return 0;
		}
		else
		{
			kcout << "PM is already present.\r";

			kcout << "PhysicalAddress: " << hex_number(pde->Pte[pml4_index].PhysicalAddress);
			kcout << "\r";

			kcout << "User: " << (pde->Pte[pml4_index].User ? "YES" : "NO") << "\r";
			kcout << "RW: " << (pde->Pte[pml4_index].Rw ? "YES" : "NO") << "\r";

			return 1;
		}

		return 0;
	}

	/// @brief Map address to PDE.
	/// @param pde
	/// @param phys_addr
	/// @param virt_addr
	/// @param flags
	inline void ke_unmap_address(PDE* pde, UIntPtr phys_addr, UIntPtr virt_addr, UInt32 flags)
	{
		UInt16 pml4_index = (virt_addr >> 39) & 0x1FF;

		if (pde->Pte[pml4_index].Present)
		{
			pde->Pte[pml4_index].Present		 = false;
			pde->Pte[pml4_index].PhysicalAddress = 0;
			pde->Pte[pml4_index].Rw				 = 0;
			pde->Pte[pml4_index].User			 = 0;
			pde->Pte[pml4_index].ExecDisable	 = 0;
		}
	}

	EXTERN_C UChar	In8(UInt16 port);
	EXTERN_C UShort In16(UInt16 port);
	EXTERN_C UInt	In32(UInt16 port);

	EXTERN_C void Out16(UShort port, UShort byte);
	EXTERN_C void Out8(UShort port, UChar byte);
	EXTERN_C void Out32(UShort port, UInt byte);

	EXTERN_C void rt_wait_400ns();
	EXTERN_C void rt_halt();
	EXTERN_C void rt_cli();
	EXTERN_C void rt_sti();
	EXTERN_C void rt_cld();
	EXTERN_C void rt_std();

	struct PACKED Register64 final
	{
		UShort	Limit;
		UIntPtr Base;
	};

	struct PACKED RegisterGDT final
	{
		UShort	Limit;
		UIntPtr Base;
	};

	using RawRegister = UInt64;

	using InterruptId		= UShort; /* For each element in the IVT */
	using InterruptTrapKind = UIntPtr(UIntPtr sp);

	typedef UIntPtr Reg;

	struct PACKED StackFrame final
	{
		Reg IntNum, Exception;
		Reg A0, A2, BP, SP, A3, A4, A5, A6;
		Reg R8, R9, R10, R11, R12, R13, R14, R15;
		Reg Gs, Fs;
	};

	typedef StackFrame* StackFramePtr;

	class InterruptDescriptor final
	{
	public:
		UShort Offset;
		UShort Selector;
		UChar  Ist;
		UChar  Atrributes;

		UShort SecondOffset;
		UInt   ThirdOffset;
		UInt   Zero;

		operator bool()
		{
			return Offset != 0xFFFF;
		}
	};

	using InterruptDescriptorArray = Array<InterruptDescriptor, 256>;

	class SegmentDescriptor final
	{
	public:
		UInt16 Base;
		UInt8  BaseMiddle;
		UInt8  BaseHigh;

		UShort Limit;
		UChar  Gran;
		UChar  AccessByte;
	};

	/***
	 * @brief Segment Boolean operations
	 */
	class SegmentDescriptorComparator final
	{
	public:
		bool IsValid(SegmentDescriptor& seg)
		{
			return seg.Base > seg.Limit;
		}
		bool Equals(SegmentDescriptor& seg, SegmentDescriptor& segRight)
		{
			return seg.Base == segRight.Base && seg.Limit == segRight.Limit;
		}
	};

	using SegmentArray = Array<SegmentDescriptor, 6>;

	class GDTLoader final
	{
	public:
		static void Load(RegisterGDT& gdt);
		static void Load(Ref<RegisterGDT>& gdt);
	};

	class IDTLoader final
	{
	public:
		static void Load(Register64& idt);
		static void Load(Ref<Register64>& idt);
	};

	Void hal_system_get_cores(VoidPtr rsdPtr);
	Void hal_send_start_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress);
	Void hal_send_end_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress);

	inline Bool hal_has_msr() noexcept
	{
		static UInt32 eax, unused, edx; // eax, edx

		__get_cpuid(1, &eax, &unused, &unused, &edx);

		// edx returns the flag for MSR (which is 1 shifted to 5.)
		return edx & (1 << 5);
	}

	inline Void hal_get_msr(UInt32 msr, UInt32* lo, UInt32* hi) noexcept
	{
		asm volatile("rdmsr"
					 : "=a"(*lo), "=d"(*hi)
					 : "c"(msr));
	}

	inline Void hal_set_msr(UInt32 msr, UInt32 lo, UInt32 hi) noexcept
	{
		asm volatile("wrmsr"
					 :
					 : "a"(lo), "d"(hi), "c"(msr));
	}

	/// @brief Processor specific structures.
	namespace Detail
	{
		EXTERN_C void _ke_power_on_self_test(void);

		/**
	@brief Global descriptor table entry, either null, code or data.
*/

		struct PACKED NewOSGDTRecord final
		{
			UInt16 Limit0;
			UInt16 Base0;
			UInt8  Base1;
			UInt8  AccessByte;
			UInt8  Limit1_Flags;
			UInt8  Base2;
		};

		struct PACKED ALIGN(0x1000) NewOSGDT final
		{
			NewOSGDTRecord Null;
			NewOSGDTRecord KernCode;
			NewOSGDTRecord KernData;
			NewOSGDTRecord UserNull;
			NewOSGDTRecord UserCode;
			NewOSGDTRecord UserData;
		};
	} // namespace Detail
} // namespace Kernel::HAL

EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp);
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp);
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp);
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp);

EXTERN_C void hal_load_idt(Kernel::HAL::Register64 ptr);
EXTERN_C void hal_load_gdt(Kernel::HAL::RegisterGDT ptr);

/// @brief Maximum size of the IDT.
#define kKernelIdtSize	   0x100
#define kKernelInterruptId 0x32

inline Kernel::VoidPtr kKernelVirtualStart = (Kernel::VoidPtr)cHeapStartOffset;
inline Kernel::UIntPtr kKernelVirtualSize  = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;
