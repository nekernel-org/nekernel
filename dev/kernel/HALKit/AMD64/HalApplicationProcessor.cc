/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#define APIC_MAG "APIC"

#define APIC_ICR_LOW 0x300
#define APIC_ICR_HIGH 0x310
#define APIC_SIPI_VEC 0x00500
#define APIC_EIPI_VEC 0x00400

#define LAPIC_REG_TIMER_LVT 0x320
#define LAPIC_REG_TIMER_INITCNT 0x380
#define LAPIC_REG_TIMER_CURRCNT 0x390
#define LAPIC_REG_TIMER_DIV 0x3E0
#define LAPIC_REG_ENABLE 0x80
#define LAPIC_REG_SPURIOUS 0xF0

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BSP 0x100
#define APIC_BASE_MSR_ENABLE 0x800

#include <ArchKit/ArchKit.h>
#include <HALKit/AMD64/Processor.h>
#include <KernelKit/BinaryMutex.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/Timer.h>
#include <NeKit/KernelPanic.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/CoreGfx/TextGfx.h>

///////////////////////////////////////////////////////////////////////////////////////

/// @note: _hal_switch_context is internal.
/// @brief The **HAL** namespace.

///////////////////////////////////////////////////////////////////////////////////////

namespace Kernel::HAL {
struct HAL_APIC_MADT;
struct HAL_HARDWARE_THREAD;

struct HAL_HARDWARE_THREAD final {
  StackFramePtr mFramePtr;
  ProcessID     mThreadID{0};
};

EXTERN_C Void sched_jump_to_task(StackFramePtr stack_frame);

STATIC HAL_APIC_MADT* kSMPBlock = nullptr;
STATIC Bool           kSMPAware = false;
STATIC Int64          kSMPCount = 0;

EXTERN_C UIntPtr kApicBaseAddress;

STATIC Int32   kSMPInterrupt                   = 0;
STATIC UInt64  kAPICLocales[kMaxAPInsideSched] = {0};
STATIC VoidPtr kRawMADT                        = nullptr;

STATIC HAL_HARDWARE_THREAD kHWThread[kSchedProcessLimitPerTeam] = {{}};

/// @brief Multiple APIC Descriptor Table.
struct HAL_APIC_MADT final SDT_OBJECT {
  UInt32 Address;  // Madt address
  UInt32 Flags;    // Madt flags
  UInt8  List[1];  // Records List
};

/// @brief Local APIC Descriptor Table.
struct LAPIC final {
  UInt8  Type;
  UInt8  Length;
  UInt8  ProcessorID;
  UInt8  APICID;
  UInt32 Flags;
};

///////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************/
/// @brief Send end IPI for CPU.
/// @param apic_id
/// @param vector
/// @param target
/// @return
/***********************************************************************************/
Void hal_send_ipi_msg(UInt32 target, UInt32 apic_id, UInt8 vector) {
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_HIGH, apic_id << 24);
  Kernel::ke_dma_write<UInt32>(target, APIC_ICR_LOW, 0x00000600 | 0x00004000 | 0x00000000 | vector);

  while (Kernel::ke_dma_read<UInt32>(target, APIC_ICR_LOW) & 0x1000) {
    NE_UNUSED(0);
  }
}

/***********************************************************************************/
/// @brief Get current stack frame for a thread.
/// @param thrdid The thread ID.
/***********************************************************************************/

EXTERN_C HAL::StackFramePtr mp_get_current_task(ThreadID thrdid) {
  if (thrdid > kSMPCount) return nullptr;
  return kHWThread[thrdid].mFramePtr;
}

/***********************************************************************************/
/// @brief Register current stack frame for a thread.
/// @param stack_frame The current stack frame.
/// @param thrdid The thread ID.
/***********************************************************************************/

EXTERN_C BOOL mp_register_task(HAL::StackFramePtr stack_frame, ThreadID thrdid) {
  if (!stack_frame) return NO;

  if (thrdid > kSMPCount) return NO;

  if (!kSMPAware) {
    sched_jump_to_task(kHWThread[thrdid].mFramePtr);

    return YES;
  }

  HardwareThreadScheduler::The()[thrdid].Leak()->Busy(NO);
  kHWThread[thrdid].mFramePtr = stack_frame;

  return YES;
}

/***********************************************************************************/
/// @brief Is the current config SMP aware?
/// @return True if YES, False if not.
/***********************************************************************************/

Bool mp_is_smp(Void) noexcept {
  return kSMPAware;
}

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

  auto pwr = hw_and_pow_int.Find(APIC_MAG);

  if (pwr.HasError()) {
    kSMPAware = NO;
    return;
  }

  kRawMADT  = pwr.Leak().Leak();
  kSMPBlock = reinterpret_cast<HAL_APIC_MADT*>(kRawMADT);
  kSMPAware = NO;

  if (kSMPBlock) {
    kSMPInterrupt = 0;
    kSMPCount     = 0;

    UInt32 lo = 0U, hi = 0U;

    hal_get_msr(APIC_BASE_MSR, &lo, &hi);

    UInt64 apic_base = ((UInt64) hi << 32) | lo;

    apic_base |= APIC_BASE_MSR_ENABLE;  // Enable APIC.

    lo = apic_base & 0xFFFFFFFF;
    hi = apic_base >> 32;

    hal_set_msr(APIC_BASE_MSR, lo, hi);

    kApicBaseAddress = apic_base & 0xFFFFF000;

    LAPICDmaWrapper controller{(VoidPtr) kApicBaseAddress};

    controller.Write(LAPIC_REG_ENABLE, 0);
    controller.Write(LAPIC_REG_SPURIOUS, 0x1FF);  // Enable bit, spurious interrupt vector register.
    controller.Write(LAPIC_REG_TIMER_DIV, 0b0011);
    controller.Write(LAPIC_REG_TIMER_LVT, 0x20 | (1 << 17));
    controller.Write(LAPIC_REG_TIMER_INITCNT, 1000000);

    volatile UInt8* entry_ptr = reinterpret_cast<volatile UInt8*>(kSMPBlock->List);
    volatile UInt8* end_ptr   = ((UInt8*) kSMPBlock) + kSMPBlock->Length;

    while (entry_ptr < end_ptr) {
      UInt8 type   = *entry_ptr;
      UInt8 length = *(entry_ptr + 1);

      // Avoid infinite loop on bad APIC tables.
      if (length < 2) break;

      if (type == 0) {
        volatile LAPIC* entry_struct = (volatile LAPIC*) entry_ptr;

        if (entry_struct->Flags & 0x1) {
          kAPICLocales[kSMPCount]        = entry_struct->ProcessorID;
          kHWThread[kSMPCount].mThreadID = kAPICLocales[kSMPCount];

          ++kSMPCount;

          kout << "Kind: LAPIC: ON\r";

          // 0x7c00, as recommended by the Intel SDM.
          hal_send_ipi_msg(kApicBaseAddress, entry_struct->ProcessorID, 0x7c);
        } else {
          kout << "Kind: LAPIC: OFF\r";
        }
      } else {
        kout << "Kind: UNKNOWN: OFF\r";
      }

      entry_ptr += length;
    }

    kSMPAware = kSMPCount > 1;
  }
}
}  // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
