/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
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
    kPartEfi = 'efi',
    kPartEpm = 'epm',
    kPartEbr = 'ebr',
};