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

#define kAPIC_Signature "APIC"

#define kAPIC_ICR_Low 0x300
#define kAPIC_ICR_High 0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

#define kAPIC_BASE_MSR 0x1B
#define kAPIC_BASE_MSR_BSP 0x100
#define kAPIC_BASE_MSR_ENABLE 0x800

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

STATIC UIntPtr kApicBaseAddress = 0UL;

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
  Kernel::ke_dma_write<UInt32>(target, kAPIC_ICR_High, apic_id << 24);
  Kernel::ke_dma_write<UInt32>(target, kAPIC_ICR_Low, 0x00000500 | 0x00004000 | 0x00000000);

  while (Kernel::ke_dma_read<UInt32>(target, kAPIC_ICR_Low) & 0x1000) {
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
  Kernel::ke_dma_write<UInt32>(target, kAPIC_ICR_High, apic_id << 24);
  Kernel::ke_dma_write<UInt32>(target, kAPIC_ICR_Low,
                               0x00000600 | 0x00004000 | 0x00000000 | vector);

  while (Kernel::ke_dma_read<UInt32>(target, kAPIC_ICR_Low) & 0x1000) {
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

  hal_send_sipi(kApicBaseAddress, kHWThread[process_index].mCoreID,
                (UInt8) (((UIntPtr) stack_frame->BP) >> 12));

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

  auto hw_and_pow_int = PowerFactoryInterface(vendor_ptr);
  kRawMADT            = hw_and_pow_int.Find(kAPIC_Signature).Leak().Leak();

  kMADTBlock = reinterpret_cast<HAL_APIC_MADT*>(kRawMADT);
  kSMPAware  = NO;

  if (kMADTBlock) {
    SizeT index = 1;

    kSMPInterrupt = 0;
    kSMPCount     = 0;

    kout << "SMP: Starting APs...\r";

    kApicBaseAddress = kMADTBlock->Address;

    while (Yes) {
      /// @note Anything bigger than x2APIC type doesn't exist.
      if (kMADTBlock->List[index].Type > 9 || kSMPCount > kSchedProcessLimitPerTeam) break;

      switch (kMADTBlock->List[index].Type) {
        case 0x00: {
          if (kMADTBlock->List[kSMPCount].Apic.LAPIC.ProcessorID < 1) break;

          kAPICLocales[kSMPCount] = kMADTBlock->List[kSMPCount].Apic.LAPIC.ProcessorID;
          (Void)(kout << "SMP: APIC ID: " << number(kAPICLocales[kSMPCount]) << kendl);

          ++kSMPCount;
          break;
        }
        default:
          break;
      }

      ++index;
    }

    (Void)(kout << "SMP: Number of APs: " << number(kSMPCount) << kendl);

    // Kernel is now SMP aware.
    // That means that the scheduler is now available (on MP Kernels)

    kSMPAware = true;

    /// TODO: Notify Boot AP that it must start.
  }
}
}  // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
