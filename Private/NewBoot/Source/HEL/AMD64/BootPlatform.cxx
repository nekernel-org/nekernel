/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>

EXTERN_C void rt_hlt() { asm volatile("hlt"); }

EXTERN_C void rt_cli() { asm volatile("cli"); }

EXTERN_C void rt_sti() { asm volatile("sti"); }

EXTERN_C void rt_cld() { asm volatile("cld"); }

EXTERN_C void rt_std() { asm volatile("std"); }