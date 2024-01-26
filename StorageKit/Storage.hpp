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

#include <StorageKit/StorageCore.inl>

typedef hCore::UInt16 OSScsiPacket[12];

extern const OSScsiPacket kCDRomPacketTemplate;

#define f_kDriveSectorSize 2048
#define f_kDriveSize(last_lba) ((last_lba + 1) * f_kDriveSectorSize)


		  
										  
