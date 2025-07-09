/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

  FILE: DiskImage+EPM.cc
  PURPOSE: Disk Imaging framework.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>

#include <FSKit/NeFS.h>
#include <FirmwareKit/EPM.h>

/// @brief EPM format disk
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DI::DIFormatPartitionEPM(struct DI_DISK_IMAGE& img) noexcept {
  if (!img.sector_sz || (img.sector_sz % 512 != 0)) return kDIFailureStatus;
  if (!img.fs_version) return kDIFailureStatus;
  if (*img.out_name == 0 || *img.disk_name == 0) return kDIFailureStatus;

  struct ::EPM_PART_BLOCK block {};

  block.NumBlocks = img.block_cnt;
  block.SectorSz  = img.sector_sz;
  block.Version   = kEPMRevisionBcd;
  block.LbaStart  = sizeof(struct ::EPM_PART_BLOCK);
  block.LbaEnd    = img.disk_sz - block.LbaStart;
  block.FsVersion = img.fs_version;

  ::MmCopyMemory(block.Name, (VoidPtr) img.disk_name, ::MmStrLen(img.disk_name));

  ::MmCopyMemory(block.Magic, (VoidPtr) kEPMMagic, ::MmStrLen(kEPMMagic));

  IORef handle = IoOpenFile(img.out_name, nullptr);

  if (!handle) return kDIFailureStatus;

  ::IoWriteFile(handle, (Char*) &block, sizeof(struct ::EPM_PART_BLOCK));
  
  ::IoCloseFile(handle);

  handle = nullptr;

  return kDISuccessStatus;
}
