/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/*
 *
 * @file Processor.cxx
 * @brief Processor Specific Functions.
 *
 */

#include <BootKit/Processor.hxx>

extern "C" void rt_halt(void) { asm volatile("hlt"); }

extern "C" void rt_cli(void) { asm volatile("cli"); }

extern "C" void rt_sti(void) { asm volatile("sti"); }

extern "C" void rt_cld(void) { asm volatile("cld"); }
