/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <DmaKit/DmaPool.h>
#include <HALKit/AMD64/Processor.h>
#include <modules/ACPI/ACPIFactoryInterface.h>

using namespace Kernel;
using namespace Kernel::HAL;

STATIC UInt16 kIOBase = 0xFFFF;

STATIC UInt32                 kRXOffset     = 0UL;
STATIC constexpr CONST UInt32 kRxBufferSize = 8192 + 16 + 1500;

STATIC BOOL kTXEnabled = NO;

STATIC UInt8* kRXUpperLayer = nullptr;
STATIC UInt8* kRxBuffer     = nullptr;

EXTERN_C Void rtl_init_nic_rtl8139(UInt16 io_base) noexcept {
  if (kTXEnabled) return;

  kIOBase = io_base;
  MUST_PASS(io_base);

  kRxBuffer = (UInt8*) rtl_dma_alloc(sizeof(UInt8) * kRxBufferSize, 0);

  MUST_PASS(kRxBuffer);

  /// Reset first.

  rt_out8(io_base + 0x37, 0x10);

  UInt16 timeout = 0U;

  while (rt_in8(io_base + 0x37) & 0x10) {
    ++timeout;
    if (timeout > 0x1000) break;
  }

  MUST_PASS(timeout <= 0x1000);

  rt_out32(io_base + 0x30, (UInt32) (UIntPtr) kRxBuffer);

  rt_out8(io_base + 0x37, 0x0C);

  rt_out32(io_base + 0x44, 0xf | (1 << 7));

  // Enable IRQ.
  rt_out16(io_base + 0x3C, 0x0005);

  kTXEnabled = YES;
}

EXTERN_C void rtl_rtl8139_interrupt_handler() {
  if (kIOBase == 0xFFFF) return;

  UInt16 status = rt_in16(kIOBase + 0x3E);
  rt_out16(kIOBase + 0x3E, status);

  if (status & 0x01) {
    while ((rt_in8(kIOBase + 0x37) & 0x01) == 0) {
      UInt32          offset = kRXOffset % kRxBufferSize;
      volatile UInt8* packet = kRxBuffer + offset + 4;
      UInt16          len    = *(UInt16*) (kRxBuffer + offset + 2);

      kRXUpperLayer[offset + 4] = *packet;

      kRXOffset += len + 4;
      rt_out16(kIOBase + 0x38, (UInt16) (kRXOffset - 16));
    }
  }

  if (!(status & 0x04)) {
    err_global_get() = kErrorNoNetwork;
  }
}