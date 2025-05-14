/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

  FILE: DiskImage+NeFS.cc
  PURPOSE: Disk Imaging framework.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>

#include <FSKit/NeFS.h>
#include <FirmwareKit/EPM.h>

/// @brief format NeFS over an EPM disk.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DI::DIFormatFilesystemNeFS(struct DI_DISK_IMAGE& img) noexcept {
  if (!img.sector_sz || (img.sector_sz % kDISectorSz != 0)) return kDIFailureStatus;

  if (*img.out_name == 0 || *img.disk_name == 0) return kDIFailureStatus;

  struct ::NEFS_ROOT_PARTITION_BLOCK rpb {};

  ::MmCopyMemory(rpb.PartitionName, (VoidPtr) img.disk_name, ::MmStrLen(img.disk_name));
  ::MmCopyMemory(rpb.Ident, (VoidPtr) kNeFSIdent, ::MmStrLen(kNeFSIdent));

  rpb.Version  = kNeFSVersionInteger;
  rpb.EpmBlock = kEPMBootBlockLba;

  rpb.StartCatalog = kNeFSCatalogStartAddress;
  rpb.CatalogCount = 0;

  rpb.DiskSize = img.disk_sz;

  rpb.SectorSize  = img.sector_sz;
  rpb.SectorCount = rpb.DiskSize / rpb.SectorSize;

  rpb.FreeSectors = rpb.SectorCount;
  rpb.FreeCatalog = rpb.DiskSize / sizeof(NEFS_CATALOG_STRUCT);

  IORef handle = IoOpenFile(img.out_name, nullptr);

  if (!handle) return kDIFailureStatus;

  UInt64 p_prev = ::IoTellFile(handle);

  ::IoWriteFile(handle, (Char*) &rpb, sizeof(struct ::NEFS_ROOT_PARTITION_BLOCK));

  ::IoSeekFile(handle, p_prev);

  ::IoCloseFile(handle);

  handle = nullptr;

  return kDISuccessStatus;
}