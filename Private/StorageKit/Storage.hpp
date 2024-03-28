/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <StorageKit/NVME.hpp>
#include <StorageKit/AHCI.hpp>

typedef NewOS::UInt16 SKScsiPacket[12];

extern const SKScsiPacket kCDRomPacketTemplate;

#define f_kDriveSectorSizeHDD (512)
#define f_kDriveSectorSizeSSD (4096)
#define f_kDriveSectorSizeCDROM (2048)

#define f_kDriveSize(LAST_LBA) ((LAST_LBA + 1) * f_kDriveSectorSize)

#include <StorageKit/StorageCore.inl>

