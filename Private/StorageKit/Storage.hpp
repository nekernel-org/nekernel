/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <StorageKit/ATA.hpp>
#include <StorageKit/NVME.hpp>

typedef HCore::UInt16 SKScsiPacket[12];

extern const SKScsiPacket kCDRomPacketTemplate;

#define f_kDriveSectorSize 512
#define f_kDriveSize(last_lba) ((last_lba + 1) * f_kDriveSectorSize)

#include <StorageKit/StorageCore.inl>
