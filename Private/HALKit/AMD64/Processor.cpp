/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <HALKit/AMD64/Processor.hpp>

namespace hCore::HAL {
    void out8(UInt16 port, UInt8 value) {
        asm volatile("outb %%al, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    void out16(UInt16 port, UInt16 value) {
        asm volatile("outw %%ax, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    void out32(UInt16 port, UInt32 value) {
        asm volatile("outl %%eax, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    UInt8 in8(UInt16 port) {
        UInt8 value = 0UL;
        asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

        return value;
    }

    UInt16 in16(UInt16 port) {
        UInt16 value = 0UL;
        asm volatile("inw %1, %%ax" : "=a"(value) : "Nd"(port) : "memory");

        return value;
    }

    UInt32 in32(UInt16 port) {
        UInt32 value = 0UL;
        asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

        return value;
    }

    void rt_halt() {
        asm volatile("hlt");
    }

    void rt_cli() {
        asm volatile("cli");
    }

    void rt_sti() {
        asm volatile("sti");
    }

    void rt_cld() {
        asm volatile("cld");
    }
} // namespace hCore::HAL
