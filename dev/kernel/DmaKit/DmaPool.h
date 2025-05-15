/*
 * Copyright (c) 2025 Amlal El Mahrouss. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <HALKit/AMD64/Processor.h>
#include <KernelKit/DebugOutput.h>

#ifdef __NE_AMD64__
#define kNeDMAPoolStart (0x1000000)
#define kNeDMAPoolSize (0x1000000)

namespace Kernel {
/// @brief DMA pool base pointer, here we're sure that AHCI or whatever tricky standard sees it.
inline UInt8*       kDmaPoolPtr = (UInt8*) kNeDMAPoolStart;
inline const UInt8* kDmaPoolEnd = (UInt8*) (kNeDMAPoolStart + kNeDMAPoolSize);

/***********************************************************************************/
/// @brief allocate from the rtl_dma_alloc system.
/// @param size the size of the chunk to allocate.
/// @param align alignement of pointer.
/***********************************************************************************/
inline VoidPtr rtl_dma_alloc(SizeT size, SizeT align) {
  if (!size) {
    return nullptr;
  }

  UIntPtr addr = (UIntPtr) kDmaPoolPtr;

  /// here we just align the address according to a `align` variable, i'd rather be a power of two
  /// really.
  addr = (addr + (align - 1)) & ~(align - 1);

  if ((addr + size) >= reinterpret_cast<UIntPtr>(kDmaPoolEnd)) {
    kout << "DMA Pool is exhausted!\r";

    err_global_get() = kErrorDmaExhausted;

    return nullptr;
  }

  kDmaPoolPtr = (UInt8*) (addr + size);
  return (VoidPtr) addr;
}

/***********************************************************************************/
/// @brief Free DMA pointer.
/***********************************************************************************/
inline Void rtl_dma_free(SizeT size) {
  if (!size) return;

  kDmaPoolPtr = (UInt8*) (kDmaPoolPtr - size);
}

/***********************************************************************************/
/// @brief Flush DMA pointer.
/***********************************************************************************/
inline Void rtl_dma_flush(VoidPtr ptr, SizeT size_buffer) {
  if (ptr > kDmaPoolEnd) {
    return;
  }

  if (!ptr || ptr < (UInt8*) kNeDMAPoolStart) {
    return;
  }

  for (SizeT buf_idx = 0UL; buf_idx < size_buffer; ++buf_idx) {
    HAL::mm_memory_fence((VoidPtr) ((UInt8*) ptr + buf_idx));
  }
}
}  // namespace Kernel
#endif