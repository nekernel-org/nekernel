/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#define __MPT_NEED_ATA_SUPPORT 1

#include <BootKit/Boot.hpp>
#include <MPT/API.hxx>

extern "C" void Main(void)
{
#ifdef __DBG__

    BKTextWriter writer;
    writer.WriteString("Booting Kernel...", kBlack, kWhite, 0, 0);

#endif

    init_ata_mpt();
}
