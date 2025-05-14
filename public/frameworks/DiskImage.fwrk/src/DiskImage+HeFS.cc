/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

  FILE: DiskImage+NeFS.cc
  PURPOSE: Disk Imaging framework.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>

#include <FSKit/HeFS.h>
#include <FirmwareKit/EPM.h>

/// @brief format HeFS over an EPM disk.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DI::DIFormatFilesystemHeFS(struct DI_DISK_IMAGE& img) noexcept {
  if (!img.sector_sz || (img.sector_sz % 512 != 0)) return kDIFailureStatus;

  if (*img.out_name == 0 || *img.disk_name == 0) return kDIFailureStatus;

  IORef handle = IoOpenFile(img.out_name, nullptr);

  if (!handle) return kDIFailureStatus;

  ::IoCloseFile(handle);

  handle = nullptr;

  return kDISuccessStatus;
}