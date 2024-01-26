/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/Boot.hpp>

extern "C" void Main(void)
{
    BKTextWriter writer;
    writer.WriteString("Booting Kernel...", kBlack, kWhite, 0, 0);

}
