/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <StorageKit/ATA.hpp>
#include <StorageKit/NVME.hpp>

typedef hCore::UInt16 OSScsiPacket[12];

extern const OSScsiPacket kCDRomPacketTemplate;

#define f_kDriveSectorSize 512
#define f_kDriveSize(last_lba) ((last_lba + 1) * f_kDriveSectorSize)

#include <StorageKit/StorageCore.inl>
