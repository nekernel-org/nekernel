/* -------------------------------------------

	Copyright ZKA Technologies.

	File: Prcoessor.hxx
	Purpose: AMD64 processor abstraction.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <FirmwareKit/Handover.hxx>
#include <HALKit/AMD64/HalPageAlloc.hxx>

EXTERN_C
{
#include <cpuid.h>
}

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif // ifdef kCPUBackendName

#define kCPUBackendName "AMD64"

#define kSyscallRoute (0x32)

#define IsActiveLow(FLG)	  (FLG & 2)
#define IsLevelTriggered(FLG) (FLG & 8)

#define kInterruptGate		 (0x8E)
#define kTrapGate			 (0xEF)
#define kTaskGate			 (0b10001100)
#define kGdtCodeSelector	 (0x08)
#define kGdtUserCodeSelector (0x10)

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
		eFlagsPresent,
		eFlagsUser,
		eFlagsRw,
		eFlagsExecDisable,
		eFlagsSetPhysAddress,
		eFlagsCount,
	};

	/// @brief Updates a PTE from pd_base.
	/// @param pd_base a valid PDE address.
	/// @param phys_addr a valid phyiscal address.
	/// @param virt_addr a valid virtual address.
	/// @param flags the flags to put on the page.
	EXTERN_C Int32 mm_update_page(VoidPtr pd_base, VoidPtr phys_addr, VoidPtr virt_addr, UInt32 flags);

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
	using Reg		  = RawRegister;
	using InterruptId = UInt16; /* For each element in the IVT */

	/// @brief Stack frame (as retrieved from assembly.)
	struct PACKED StackFrame final
	{
		UIntPtr R8{0};
		UIntPtr R9{0};
		UIntPtr R10{0};
		UIntPtr R11{0};
		UIntPtr R12{0};
		UIntPtr R13{0};
		UIntPtr R14{0};
		UIntPtr R15{0};
		UIntPtr BP{0};
		UIntPtr SP{0};
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
		Bool IsValid(SegmentDescriptor& seg)
		{
			return seg.Base > seg.Limit;
		}

		Bool Equals(SegmentDescriptor& seg, SegmentDescriptor& segRight)
		{
			return seg.Base == segRight.Base && seg.Limit == segRight.Limit;
		}
	};

	using SegmentArray = Array<SegmentDescriptor, 6>;

	class GDTLoader final
	{
	public:
		static Void Load(RegisterGDT& gdt);
		static Void Load(Ref<RegisterGDT>& gdt);
	};

	class IDTLoader final
	{
	public:
		static Void Load(Register64& idt);
		static Void Load(Ref<Register64>& idt);
	};

	Void mp_get_cores(VoidPtr rsdPtr) noexcept;
	Void hal_send_start_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress);
	Void hal_send_end_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress);

	/// @brief Do a cpuid to check if MSR exists on CPU.
	/// @retval true it does exists.
	/// @retval false it doesn't.
	inline Bool hal_has_msr() noexcept
	{
		static UInt32 eax, unused, edx; // eax, edx

		__get_cpuid(1, &eax, &unused, &unused, &edx);

		// edx returns the flag for MSR (which is 1 shifted to 5.)
		return edx & (1 << 5);
	}

	/// @brief Get Model-specific register.
	/// @param msr MSR
	/// @param lo low byte
	/// @param hi high byte
	inline Void hal_get_msr(UInt32 msr, UInt32* lo, UInt32* hi) noexcept
	{
		if (!lo || !hi)
			return;

		asm volatile("rdmsr"
					 : "=a"(*lo), "=d"(*hi)
					 : "c"(msr));
	}

	/// @brief Set Model-specific register.
	/// @param msr MSR
	/// @param lo low byte
	/// @param hi high byte
	inline Void hal_set_msr(UInt32 msr, UInt32 lo, UInt32 hi) noexcept
	{
		asm volatile("wrmsr"
					 :
					 : "a"(lo), "d"(hi), "c"(msr));
	}

	/// @brief Processor specific namespace.
	namespace Detail
	{
		/**
		  @brief Global descriptor table entry, either null, code or data.
		*/

		struct PACKED ZKA_GDT_ENTRY final
		{
			UInt16 fLimit0;
			UInt16 fBase0;
			UInt8  fBase1;
			UInt8  fAccessByte;
			UInt8  fLimit1_Flags;
			UInt8  fBase2;
		};

		struct PACKED ALIGN(0x1000) ZKA_GDT final
		{
			ZKA_GDT_ENTRY fKernNull;
			ZKA_GDT_ENTRY fKernCode;
			ZKA_GDT_ENTRY fKernData;
			ZKA_GDT_ENTRY fUserNull;
			ZKA_GDT_ENTRY fUserCode;
			ZKA_GDT_ENTRY fUserData;
		};
	} // namespace Detail

	class APICController
	{
	public:
		explicit APICController(VoidPtr base)
			: fApic(base)
		{
		}

		~APICController() = default;

		ZKA_COPY_DEFAULT(APICController);

	public:
		UInt32 Read(UInt32 reg) noexcept;
		Void   Write(UInt32 reg, UInt32 value) noexcept;

	private:
		VoidPtr fApic{nullptr};
	};
} // namespace Kernel::HAL

EXTERN_C Kernel::Void idt_handle_generic(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_gpf(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_math(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_pf(Kernel::UIntPtr rsp);

EXTERN_C Kernel::Void hal_load_idt(Kernel::HAL::Register64 ptr);
EXTERN_C Kernel::Void hal_load_gdt(Kernel::HAL::RegisterGDT ptr);

/// @brief Maximum size of the IDT.
#define kKernelIdtSize	   0x100
#define kKernelInterruptId 0x32

inline Kernel::VoidPtr kKernelVMHStart	   = nullptr;
inline Kernel::VoidPtr kKernelVirtualStart = nullptr;
inline Kernel::UIntPtr kKernelVirtualSize  = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;
