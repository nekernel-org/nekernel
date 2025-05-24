/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <DmaKit/DmaPool.h>
#include <HALKit/AMD64/Processor.h>
#include <modules/ACPI/ACPIFactoryInterface.h>

using namespace Kernel;
using namespace Kernel::HAL;

STATIC UInt16 kRTLIOBase = 0xFFFF;

STATIC BOOL kTXEnabled = NO;

STATIC UInt32 kRXOffset = 0UL;

STATIC constexpr CONST UInt32 kRXBufferSize = 8192 + 16 + 1500;

STATIC UInt8* kRXUpperLayer = nullptr;
STATIC UInt8* kRXBuffer     = nullptr;

/***********************************************************************************/
///@brief RTL8139 Init routine.
/***********************************************************************************/

EXTERN_C Void rtl_init_nic_rtl8139(UInt16 io_base) noexcept {
  if (kTXEnabled) return;

  kRTLIOBase = io_base;

  MUST_PASS(io_base != 0xFFFF);

  kRXBuffer = reinterpret_cast<UInt8*>(rtl_dma_alloc(sizeof(UInt8) * kRXBufferSize, 0));

  MUST_PASS(kRXBuffer);

  /// Reset first.

  rt_out8(io_base + 0x37, 0x10);

  UInt16 timeout = 0U;

  while (rt_in8(io_base + 0x37) & 0x10) {
    ++timeout;
    if (timeout > 0x1000) break;
  }

  if (timeout <= 0x1000) {
    ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "RTL8139: Reset failed");
    return;
  }

  rt_out32(io_base + 0x30, (UInt32) (UIntPtr) kRXBuffer);

  rt_out8(io_base + 0x37, 0x0C);

  rt_out32(io_base + 0x44, 0xF | (1 << 7));

  rt_out16(io_base + 0x3C, 0x0005);

  kTXEnabled = YES;
}

/***********************************************************************************/
/// @brief RTL8139 I/O interrupt handler.
/// @note This function is called when the device interrupts to retrieve network data.
/***********************************************************************************/

EXTERN_C void rtl_rtl8139_interrupt_handler() {
  if (kRTLIOBase == 0xFFFF) return;

  UInt16 status = rt_in16(kRTLIOBase + 0x3E);
  rt_out16(kRTLIOBase + 0x3E, status);

  if (status & 0x01) {
    // While we receive data.
    while ((rt_in8(kRTLIOBase + 0x37) & 0x01) == 0) {
      // We grab an offset from the RX buffer.
      UInt32          offset = kRXOffset % kRXBufferSize;

      // If the offset is too high, we reset it.
      if (offset >= (kRXBufferSize - 16)) {
        kRXOffset = 0UL;
        offset    = 0UL;
      }

      volatile UInt8* packet = kRXBuffer + offset + 4;
      UInt16          len    = *(UInt16*) (kRXBuffer + offset + 2);

      kRXUpperLayer[(offset + 4)] = *packet;
      kRXOffset += (len + 4);

      rt_out16(kRTLIOBase + 0x38, (UInt16) (kRXOffset - 16));
    }
  }

  if (!(status & 0x04)) {
    err_global_get() = kErrorNoNetwork;
  }
}

/***********************************************************************************/
/// @brief RTL8139 get upper layer function
/// @return the upper layer.
/// @retval nullptr if no upper layer is set.
/// @retval pointer to the upper layer if set.
/***********************************************************************************/

EXTERN_C UInt8* rtl_rtl8139_get_upper_layer() {
  return kRXUpperLayer;
}