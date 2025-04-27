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

#include <KernelKit/DebugOutput.h>
#include <NewKit/Defines.h>

#ifdef __NE_AMD64__
#define DMA_POOL_START (0x1000000)
#define DMA_POOL_SIZE (0x1000000)

namespace Kernel {
inline UInt8* kDmaPoolPtr = (UInt8*) DMA_POOL_START;

inline VoidPtr rtl_dma_alloc(SizeT size, SizeT align) {
  if (!size) {
    return nullptr;
  }

  UIntPtr addr = (UIntPtr) kDmaPoolPtr;

  addr = (addr + (align - 1)) & ~(align - 1);  // Align up

  if (addr + size >= DMA_POOL_START + DMA_POOL_SIZE) {
    kout << "DMA Pool exhausted!\r";

    return nullptr;
  }

  kDmaPoolPtr = (UInt8*) (addr + size);
  return (VoidPtr) addr;
}

inline Void rtl_dma_free(SizeT size) {
  if (!size) return;

  kDmaPoolPtr = (UInt8*) (kDmaPoolPtr - size);
}

inline Void rtl_dma_flush(Void* ptr, SizeT size_buffer) {
  if (ptr > (Void*) (DMA_POOL_START + DMA_POOL_SIZE)) {
    return;
  }

  for (SizeT i = 0; i < size_buffer; ++i) {
    asm volatile("clflush (%0)" : : "r"((UInt8*) ptr + i) : "memory");
  }
  asm volatile("mfence" ::: "memory");
}
}  // namespace Kernel
#endif