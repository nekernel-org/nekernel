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

#define IsActiveLow(FLG)	  (FLG & 2)
#define IsLevelTriggered(FLG) (FLG & 8)

#define kInterruptGate (0x8E)
#define kTrapGate	   (0xEF)
#define kTaskGate	   (0b10001100)
#define kIDTSelector   (0x08)

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
		eFlagsPresent = 0x01,
		eFlagsRw	  = 0x02,
		eFlagsUser	  = 0x04,
		eFlagsCount	  = 0x3,
	};

	/// @brief Set a PTE from pd_base.
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manip.
	EXTERN_C Int32 mm_map_page(VoidPtr virt_addr, VoidPtr phys_addr, UInt32 flags);

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
		RawRegister R8{0};
		RawRegister R9{0};
		RawRegister R10{0};
		RawRegister FS{0};
		RawRegister R12{0};
		RawRegister R13{0};
		RawRegister R14{0};
		RawRegister R15{0};
		RawRegister GS{0};
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
		/* @brief TSS struct. */
		struct ZKA_TSS final
		{
			UInt32 fReserved1;
			UInt64 fRsp0;
			UInt64 fRsp1;
			UInt64 fRsp2;
			UInt64 fReserved2;
			UInt64 fIst1;
			UInt64 fIst2;
			UInt64 fIst3;
			UInt64 fIst4;
			UInt64 fIst5;
			UInt64 fIst6;
			UInt64 fIst7;
			UInt64 fReserved3;
			UInt16 fReserved4;
			UInt16 fIopb;
		};

		/**
		  @brief Global descriptor table entry, either null, code or data.
		*/

		struct PACKED ZKA_GDT_ENTRY final
		{
			UInt16 fLimitLow;
			UInt16 fBaseLow;
			UInt8  fBaseMid;
			UInt8  fAccessByte;
			UInt8  fFlags;
			UInt8  fBaseHigh;
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
#define kKernelIdtSize	   (0x100)
#define kKernelInterruptId 0x32

inline Kernel::VoidPtr kKernelVirtualStart = nullptr;
inline Kernel::UIntPtr kKernelVirtualSize  = 0UL;

inline Kernel::VoidPtr kKernelPhysicalStart = nullptr;
