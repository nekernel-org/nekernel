/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#ifdef __MPT_NEED_ATA_SUPPORT
extern "C" int init_ata_mpt(void);
#endif // __MPT_NEED_ATA_SUPPORT

#include "Detail.hxx"
#include "MPT.hxx"
