/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: HalCPU.cc
  Purpose: Platform processor routines.

------------------------------------------- */

#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>

/**
 * @file HalCPU.cc
 * @brief Common CPU API.
 */

namespace Kernel::HAL {
inline Bool hal_has_msr() noexcept {
  static UInt32 eax, unused, edx;  // eax, edx

  __get_cpuid(1, &eax, &unused, &unused, &edx);

  // edx returns the flag for MSR (which is 1 shifted to 5.)
  return edx & (1 << 5);
}

Void hal_get_msr(UInt32 msr, UInt32* lo, UInt32* hi) noexcept {
  if (!lo || !hi) return;
  asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

Void hal_set_msr(UInt32 msr, UInt32 lo, UInt32 hi) noexcept {
  asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

Void lrt_hal_out8(UInt16 port, UInt8 value) {
  asm volatile("outb %%al, %1" : : "a"(value), "Nd"(port) : "memory");
}

Void lrt_hal_out16(UInt16 port, UInt16 value) {
  asm volatile("outw %%ax, %1" : : "a"(value), "Nd"(port) : "memory");
}

Void lrt_hal_out32(UInt16 port, UInt32 value) {
  asm volatile("outl %%eax, %1" : : "a"(value), "Nd"(port) : "memory");
}

UInt8 lrt_hal_in8(UInt16 port) {
  UInt8 value = 0UL;
  asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

UInt16 lrt_hal_in16(UInt16 port) {
  UInt16 value = 0UL;
  asm volatile("inw %1, %%ax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

UInt32 lrt_hal_in32(UInt16 port) {
  UInt32 value = 0UL;
  asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

Void rt_halt() {
  asm volatile("hlt");
}

Void rt_cli() {
  asm volatile("cli");
}

Void rt_sti() {
  asm volatile("sti");
}

Void rt_cld() {
  asm volatile("cld");
}

Void rt_std() {
  asm volatile("std");
}
}  // namespace Kernel::HAL
