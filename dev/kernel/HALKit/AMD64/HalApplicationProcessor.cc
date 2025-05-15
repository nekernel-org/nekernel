/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <HALKit/AMD64/Processor.h>
#include <KernelKit/BinaryMutex.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/Timer.h>
#include <NewKit/KernelPanic.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/CoreGfx/TextGfx.h>

#define APIC_Signature "APIC"

#define APIC_ICR_Low 0x300
#define APIC_ICR_High 0x310
#define APIC_SIPI_Vector 0x00500
#define APIC_EIPI_Vector 0x00400

#define LAPIC_REG_TIMER_LVT 0x320
#define LAPIC_REG_TIMER_INITCNT 0x380
#define LAPIC_REG_TIMER_CURRCNT 0x390
#define LAPIC_REG_TIMER_DIV 0x3E0

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BSP 0x100
#define APIC_BASE_MSR_ENABLE 0x800

/// @note: _hal_switch_context is internal

///////////////////////////////////////////////////////////////////////////////////////

/// @brief The **HAL** namespace.

///////////////////////////////////////////////////////////////////////////////////////

namespace Kernel::HAL {
struct HAL_APIC_MADT;
struct HAL_HARDWARE_THREAD;

struct HAL_HARDWARE_THREAD final {
  HAL::StackFramePtr mFramePtr;
  ProcessID          mProcessID{0};
  UInt8              mCoreID{0};
};

STATIC HAL_APIC_MADT* kMADTBlock = nullptr;
STATIC Bool           kSMPAware  = false;
STATIC Int64          kSMPCount  = 0;

EXTERN_C UIntPtr kApicBaseAddress = 0UL;

STATIC Int32   kSMPInterrupt                           = 0;
STATIC UInt64  kAPICLocales[kSchedProcessLimitPerTeam] = {0};
STATIC VoidPtr kRawMADT                                = nullptr;

/// @brief Multiple APIC Descriptor Table.
struct HAL_APIC_MADT final SDT_OBJECT {
  UInt32 Address;  // Madt address
  UInt8  Flags;    // Madt flags

  struct {
    UInt8 Type;
    UInt8 Len;

    union APIC {
      struct IOAPIC {
        UInt8  IoID;
        UInt8  Zero;
        UInt32 IoAddress;
        UInt32 GISBase;
      } IOAPIC;

      struct LAPIC_NMI {
        UInt8  Source;
        UInt8  IRQSource;
        UInt32 GSI;
        UInt16 Flags;
      } LApicNMI;

      struct LAPIC {
        UInt8  ProcessorID;
        UInt16 Flags;
        UInt8  LINT;
      } LAPIC;

      struct LAPIC_OVERRIDE {
        UInt16 Reserved;
        UInt64 Address;
      } LApicOverride;

      struct LAPIC_X2 {
        UInt16 Reserved;
        UInt32 x2APICID;
        UInt32 Flags;
        UInt32 AcpiID;
      } LocalApicX2;
    } Apic;
  } List[1];  // Records List
};

///////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************/
/// @brief Send IPI command to APIC.
/// @param apic_id programmable interrupt controller id.
/// @param vector vector interrupt.
/// @param target target APIC adress.
/// @return
/***********************************************************************************/

Void hal_send_start_ipi(UInt32 target, UInt32 apic_id) {
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_High, apic_id << 24);
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_Low, 0x00000500 | 0x00004000 | 0x00000000);

  while (Kernel::ke_dma_read<UInt32>(target, APIC_ICR_Low) & 0x1000) {
    ;
  }
}

/***********************************************************************************/
/// @brief Send end IPI for CPU.
/// @param apic_id
/// @param vector
/// @param target
/// @return
/***********************************************************************************/
Void hal_send_sipi(UInt32 target, UInt32 apic_id, UInt8 vector) {
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_High, apic_id << 24);
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_Low, 0x00000600 | 0x00004000 | 0x00000000 | vector);

  while (Kernel::ke_dma_read<UInt32>(target, APIC_ICR_Low) & 0x1000) {
    NE_UNUSED(0);
  }
}

STATIC HAL_HARDWARE_THREAD kHWThread[kSchedProcessLimitPerTeam] = {{}};

EXTERN_C HAL::StackFramePtr mp_get_current_context(Int64 pid) {
  const auto process_index = pid % kSchedProcessLimitPerTeam;

  return kHWThread[process_index].mFramePtr;
}

EXTERN_C BOOL mp_register_process(HAL::StackFramePtr stack_frame, ProcessID pid) {
  MUST_PASS(stack_frame);

  const auto process_index = pid % kSchedProcessLimitPerTeam;

  kHWThread[process_index].mFramePtr  = stack_frame;
  kHWThread[process_index].mProcessID = pid;

  kHWThread[process_index].mCoreID = kAPICLocales[0];

  if (mp_is_smp()) {
    /// TODO:

    return YES;
  }

  return NO;
}

/***********************************************************************************/
/// @brief Is the current config SMP aware?
/// @return True if YES, False if not.
/***********************************************************************************/
Bool mp_is_smp(Void) noexcept {
  return kSMPAware;
}

/***********************************************************************************/
/// @brief Assembly symbol to bootstrap AP.
/***********************************************************************************/
EXTERN_C Char* hal_ap_blob_start;

/***********************************************************************************/
/// @brief Assembly symbol to bootstrap AP.
/***********************************************************************************/
EXTERN_C Char* hal_ap_blob_end;

/***********************************************************************************/
/// @brief Fetch and enable SMP scheduler.
/// @param vendor_ptr SMP containing structure.
/***********************************************************************************/
Void mp_init_cores(VoidPtr vendor_ptr) noexcept {
  if (!vendor_ptr) return;

  if (!kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled) {
    kSMPAware = NO;
    return;
  }

  PowerFactoryInterface hw_and_pow_int{vendor_ptr};

  kRawMADT   = hw_and_pow_int.Find(APIC_Signature).Leak().Leak();
  kMADTBlock = reinterpret_cast<HAL_APIC_MADT*>(kRawMADT);
  kSMPAware  = NO;

  if (kMADTBlock) {
    SizeT index = 0;

    kSMPInterrupt = 0;
    kSMPCount     = 0;

    UInt32 lo = 0, hi = 0;

    hal_get_msr(0x1B, &lo, &hi);
    UInt64 apic_base = ((UInt64) hi << 32) | lo;

    apic_base |= 0x800;  // enable bit

    lo = apic_base & 0xFFFFFFFF;
    hi = apic_base >> 32;

    hal_set_msr(0x1B, lo, hi);

    kApicBaseAddress = apic_base & 0xFFFFF000;

    // Allow LAPIC to forward interrupts (TPR = 0)
    *(volatile UInt32*) (kApicBaseAddress + 0x80) = 0;

    // Set Spurious Interrupt Vector and enable LAPIC (bit 8)
    *(volatile UInt32*) (kApicBaseAddress + 0xF0) = 0x1FF;  // vector = 0xFF, enable bit = 1 << 8

    // LAPIC timer setup
    *(volatile UInt32*) (kApicBaseAddress + LAPIC_REG_TIMER_DIV) = 0b0011;  // Divide by 16

    *(volatile UInt32*) (kApicBaseAddress + LAPIC_REG_TIMER_LVT) =
        32 | (1 << 17);  // Vector 32, periodic

    *(volatile UInt32*) (kApicBaseAddress + LAPIC_REG_TIMER_INITCNT) =
        1000000;  // Init count (e.g., ~100Hz)

    constexpr const auto kSMPCountMax = kMaxAPInsideSched;

    while (Yes) {
      // @note Don't probe greater than what the APMgr expects.
      if (kSMPCount > kSMPCountMax) break;

      /// @note Anything bigger than x2APIC type doesn't exist.
      if (kMADTBlock->List[index].Type > 9) {
        ++index;
        continue;
      }

      kAPICLocales[kSMPCount] = kMADTBlock->List[kSMPCount].Apic.LAPIC.ProcessorID;
      (Void)(kout << "SMP: APIC ID: " << number(kAPICLocales[kSMPCount]) << kendl);

      ++kSMPCount;

      ++index;
    }

    kSMPAware = kSMPCount > 1;
  }
}
}  // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
