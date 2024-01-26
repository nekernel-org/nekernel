/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// @brief 255 size partition header.
// we use that to gather information about this hard drive.

struct MasterPartitionTable final
{
    char fPartName[32];
    int  fPartType;
    int  fPartSectorSz;
    int  fPartSectorCnt;
    char fReserved[211];
};

enum
{
    kPartEfi = 0x10,
    kPartEpm = 0x11,
    kPartEbr = 0x12,
};