/* -------------------------------------------

    Copyright Mahrouss Logic

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

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif // ifdef kCPUBackendName

#define kCPUBackendName "AMD64"

#define IsActiveLow(FLG)	  (FLG & 2)
#define IsLevelTriggered(FLG) (FLG & 8)

#define kInterruptGate			   (0x8E)
#define kTrapGate				   (0xEF)
#define kTaskGate				   (0b10001100)
#define kGdtCodeSelector		   (0x08)
#define kVirtualAddressStartOffset (0x10000000)

namespace NewOS
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
} // namespace NewOS

namespace NewOS::HAL
{
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

	using InterruptId	= UShort; /* For each element in the IVT */
	using interruptTrap = UIntPtr(UIntPtr sp);

	typedef UIntPtr Reg;

	struct PACKED StackFrame final
	{
		Reg IntNum, Exception;
		Reg Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
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
} // namespace NewOS::HAL

EXTERN_C void idt_handle_generic(NewOS::UIntPtr rsp);
EXTERN_C void idt_handle_gpf(NewOS::UIntPtr rsp);
EXTERN_C void idt_handle_math(NewOS::UIntPtr rsp);
EXTERN_C void idt_handle_pf(NewOS::UIntPtr rsp);

EXTERN_C void hal_load_idt(NewOS::HAL::Register64 ptr);
EXTERN_C void hal_load_gdt(NewOS::HAL::RegisterGDT ptr);

/// @brief Maximum size of the IDT.
#define kKernelIdtSize	   0x100
#define kKernelInterruptId 0x32

inline NewOS::VoidPtr kKernelVirtualStart = (NewOS::VoidPtr)kVirtualAddressStartOffset;
inline NewOS::UIntPtr kKernelVirtualSize  = 0UL;

inline NewOS::VoidPtr kKernelPhysicalStart = nullptr;
