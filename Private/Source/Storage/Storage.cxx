/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <StorageKit/Storage.hpp>

const OSScsiPacket kCDRomPacketTemplate = {0x43, 0,  1,    0, 0, 0,
                                           0,    12, 0x40, 0, 0};
