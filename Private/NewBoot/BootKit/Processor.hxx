/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

/**
    @file Processor.hxx
    @brief Processor specific code.
*/

#ifdef __x86_64__

extern "C" void rt_halt();
extern "C" void rt_cli();
extern "C" void rt_sti();
extern "C" void rt_cld();

#endif /* ifdef __x86_64__ */
