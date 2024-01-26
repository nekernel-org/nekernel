/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/Boot.hpp>

extern "C" void __AppMain(void)
{
    BTextWriter writer;
    writer.WriteString("Starting hCore...", kBlack, kWhite, 0, 0);
    
    const char* args[] = { "/hCore.bin" };
    const char* envp[] = { "/"  };
}
