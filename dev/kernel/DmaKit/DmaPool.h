/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss , all rights reserved.

  File: DmaPool.h
  Purpose: Dma Pool Manager.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.h>

#ifdef __NE_AMD64__
#include <HALKit/AMD64/Processor.h>
#define kNeDMAPoolStart (0x1000000)
#define kNeDMAPoolSize (0x1000000)
#elif defined(__NE_ARM64__)
#include <HALKit/ARM64/Processor.h>

/// @todo what reference offset shall we use?
#define kNeDMAPoolStart (0x1000000)
#define kNeDMAPoolSize (0x1000000)
#endif

#define kNeDMABestAlign __BIGGEST_ALIGNMENT__

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

  /// Check alignement according to architecture.
  if ((align % kNeDMABestAlign) != 0) {
    return nullptr;
  }

  UIntPtr addr = (UIntPtr) kDmaPoolPtr;

  /// here we just align the address according to a `align` variable, i'd rather be a power of two
  /// really.
  addr = (addr + (align - 1)) & ~(align - 1);

  if ((addr + size) >= reinterpret_cast<UIntPtr>(kDmaPoolEnd)) {
    err_global_get() = kErrorDmaExhausted;
    return nullptr;
  }

  kDmaPoolPtr = (UInt8*) (addr + size);

  HAL::mm_memory_fence((VoidPtr) addr);

  return (VoidPtr) addr;
}

/***********************************************************************************/
/// @brief Free DMA pointer.
/***********************************************************************************/
inline Void rtl_dma_free(SizeT size) {
  if (!size) return;

  auto ptr = (UInt8*) (kDmaPoolPtr - size);

  if (!ptr || ptr < (UInt8*) kNeDMAPoolStart) {
    err_global_get() = kErrorDmaExhausted;
    return;
  }

  kDmaPoolPtr = ptr;

  HAL::mm_memory_fence(ptr);
}

/***********************************************************************************/
/// @brief Flush DMA pointer.
/***********************************************************************************/
inline Void rtl_dma_flush(VoidPtr ptr, SizeT size_buffer) {
  if (ptr > kDmaPoolEnd) {
    return;
  }

  if (!ptr || ptr < (UInt8*) kNeDMAPoolStart) {
    err_global_get() = kErrorDmaExhausted;
    return;
  }

  for (SizeT buf_idx = 0UL; buf_idx < size_buffer; ++buf_idx) {
    HAL::mm_memory_fence((VoidPtr) ((UInt8*) ptr + buf_idx));
  }
}
}  // namespace Kernel