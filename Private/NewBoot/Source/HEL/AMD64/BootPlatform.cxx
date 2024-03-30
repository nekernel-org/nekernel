/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

extern "C" void rt_halt() { asm volatile("hlt"); }

extern "C" void rt_cli() { asm volatile("cli"); }

extern "C" void rt_sti() { asm volatile("sti"); }

extern "C" void rt_cld() { asm volatile("cld"); }

extern "C" void rt_std() { asm volatile("std"); }