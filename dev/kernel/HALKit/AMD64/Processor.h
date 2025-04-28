/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: Prcoessor.h
  Purpose: AMD64 processor abstraction.

  Revision History:

  30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <FirmwareKit/Handover.h>
#include <HALKit/AMD64/Paging.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Utils.h>

#define kPITControlPort (0x43)
#define kPITChannel0Port (0x40)
#define kPITFrequency (1193180)

#define kPICCommand (0x20)
#define kPICData (0x21)
#define kPIC2Command (0xA0)
#define kPIC2Data (0xA1)

#include <HALKit/AMD64/CPUID.h>

#define rtl_nop_op() asm volatile("nop")

/// @brief Maximum entries of the interrupt descriptor table.
#define kKernelIdtSize (0x100)

/// @brief interrupt for system call.
#define kKernelInterruptId (0x32)

#define IsActiveLow(FLG) (FLG & 2)
#define IsLevelTriggered(FLG) (FLG & 8)

#define kInterruptGate (0x8E)
#define kTrapGate (0xEF)
#define kTaskGate (0b10001100)
#define kIDTSelector (0x08)

namespace Kernel {
namespace Detail::AMD64 {
  struct PACKED InterruptDescriptorAMD64 final {
    UInt16 OffsetLow;  // offset bits 0..15
    UInt16 Selector;   // a code segment selector in GDT or LDT
    UInt8  Ist;
    UInt8  TypeAttributes;
    UInt16 OffsetMid;
    UInt32 OffsetHigh;
    UInt32 Zero;  // reserved
  };
}  // namespace Detail::AMD64
}  // namespace Kernel

namespace Kernel::HAL {
/// @brief Memory Manager mapping flags.
enum {
  kMMFlagsInvalid = 1 << 0,
  kMMFlagsPresent = 1 << 1,
  kMMFlagsWr      = 1 << 2,
  kMMFlagsUser    = 1 << 3,
  kMMFlagsNX      = 1 << 4,
  kMMFlagsPCD     = 1 << 5,
  kMMFlagsPwt     = 1 << 6,
  kMMFlagsCount   = 4,
};

struct PACKED Register64 final {
  UShort  Limit;
  UIntPtr Base;
};

using RawRegister = UInt64;
using Reg         = RawRegister;
using InterruptId = UInt16; /* For each element in the IVT */

/// @brief Stack frame (as retrieved from assembly.)
struct PACKED StackFrame final {
  RawRegister R8{0};
  RawRegister R9{0};
  RawRegister R10{0};
  RawRegister FS{0};
  RawRegister R12{0};
  RawRegister R13{0};
  RawRegister R14{0};
  RawRegister R15{0};
  RawRegister GS{0};
  RawRegister SP{0};
  RawRegister BP{0};
};

typedef StackFrame* StackFramePtr;

class InterruptDescriptor final {
 public:
  UShort Offset;
  UShort Selector;
  UChar  Ist;
  UChar  Atrributes;

  UShort SecondOffset;
  UInt   ThirdOffset;
  UInt   Zero;

  operator bool() { return Offset != 0xFFFF; }
};

using InterruptDescriptorArray = Array<InterruptDescriptor, 256>;

class SegmentDescriptor final {
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
class SegmentDescriptorComparator final {
 public:
  Bool IsValid(SegmentDescriptor& seg) { return seg.Base > seg.Limit; }

  Bool Equals(SegmentDescriptor& seg, SegmentDescriptor& segRight) {
    return seg.Base == segRight.Base && seg.Limit == segRight.Limit;
  }
};

using SegmentArray = Array<SegmentDescriptor, 6>;

class GDTLoader final {
 public:
  static Void Load(Register64& gdt);
  static Void Load(Ref<Register64>& gdt);
};

class IDTLoader final {
 public:
  static Void Load(Register64& idt);
  static Void Load(Ref<Register64>& idt);
};

/***********************************************************************************/
/// @brief Is the current config SMP aware?
/// @return True if YES, False if not.
/***********************************************************************************/

Bool mp_is_smp(Void) noexcept;

/***********************************************************************************/
/// @brief Fetch and enable SMP scheduler.
/// @param vendor_ptr SMP containing structure.
/***********************************************************************************/
Void mp_init_cores(VoidPtr vendor_ptr) noexcept;

/***********************************************************************************/
/// @brief Do a cpuid to check if MSR exists on CPU.
/// @retval true it does exists.
/// @retval false it doesn't.
/***********************************************************************************/
inline Bool hal_has_msr() noexcept {
  static UInt32 eax, unused, edx;  // eax, edx

  __get_cpuid(1, &eax, &unused, &unused, &edx);

  // edx returns the flag for MSR (which is 1 shifted to 5.)
  return edx & (1 << 5);
}

UIntPtr hal_get_phys_address(VoidPtr virtual_address);

/***********************************************************************************/
/// @brief Get Model specific register inside core.
/// @param msr MSR
/// @param lo low byte
/// @param hi high byte
/***********************************************************************************/
inline UInt32 hal_get_msr(UInt32 msr, UInt32* lo, UInt32* hi) noexcept {
  if (!lo || !hi) return 0;

  asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));

  return *lo + *hi;
}

/// @brief Set Model-specific register.
/// @param msr MSR
/// @param lo low byte
/// @param hi high byte
Void hal_set_msr(UInt32 msr, UInt32 lo, UInt32 hi) noexcept;

/// @brief Processor specific namespace.
namespace Detail {
  /* @brief TSS struct. */
  struct NE_TSS final {
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

  struct PACKED NE_GDT_ENTRY final {
    UInt16 fLimitLow;
    UInt16 fBaseLow;
    UInt8  fBaseMid;
    UInt8  fAccessByte;
    UInt8  fFlags;
    UInt8  fBaseHigh;
  };
}  // namespace Detail

class APICController final {
 public:
  explicit APICController(VoidPtr base);
  ~APICController() = default;

  NE_COPY_DEFAULT(APICController)

 public:
  UInt32 Read(UInt32 reg) noexcept;
  Void   Write(UInt32 reg, UInt32 value) noexcept;

 private:
  VoidPtr fApic{nullptr};
};

/// @brief Set a PTE from pd_base.
/// @param virt_addr a valid virtual address.
/// @param phys_addr point to physical address.
/// @param flags the flags to put on the page.
/// @return Status code of page manip.
EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags);

EXTERN_C UInt8  rt_in8(UInt16 port);
EXTERN_C UInt16 rt_in16(UInt16 port);
EXTERN_C UInt32 rt_in32(UInt16 port);

EXTERN_C Void rt_out16(UShort port, UShort byte);
EXTERN_C Void rt_out8(UShort port, UChar byte);
EXTERN_C Void rt_out32(UShort port, UInt byte);

EXTERN_C Void rt_wait_400ns();
EXTERN_C Void rt_halt();
EXTERN_C Void rt_cli();
EXTERN_C Void rt_sti();
EXTERN_C Void rt_cld();
EXTERN_C Void rt_std();

EXTERN_C Int32 mm_memory_fence(VoidPtr virtual_address);
}  // namespace Kernel::HAL

EXTERN_C Kernel::Void idt_handle_generic(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_gpf(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_math(Kernel::UIntPtr rsp);
EXTERN_C Kernel::Void idt_handle_pf(Kernel::UIntPtr rsp);

EXTERN_C ATTRIBUTE(naked) Kernel::Void hal_load_idt(Kernel::HAL::Register64 ptr);
EXTERN_C ATTRIBUTE(naked) Kernel::Void hal_load_gdt(Kernel::HAL::Register64 ptr);

inline Kernel::VoidPtr kKernelBitMpStart = nullptr;
inline Kernel::UIntPtr kKernelBitMpSize  = 0UL;
