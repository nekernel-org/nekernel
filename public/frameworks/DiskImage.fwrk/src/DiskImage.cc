/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.cc
	PURPOSE: Disk Imaging.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>

SInt32 DIFormatDiskToFile(const char* kDiskName,
						  int		  kDiskSectorSz,
						  const int	  kDiskBlockCnt,
						  size_t	  kDiskSz,
						  const char* kOutDisk) noexcept
{
	struct ::EPM_PART_BLOCK block
	{
		0
	};

	block.NumBlocks = kDiskBlockCnt;
	block.SectorSz	= kDiskSectorSz;
	block.Version	= kEPMRevisionBcd;
	block.LbaStart	= sizeof(struct ::EPM_PART_BLOCK);
	block.LbaEnd	= kDiskSz - block.LbaStart;
	block.FsVersion = kNeFSVersionInteger;

	::MmCopyMemory(block.Name, (VoidPtr)kDiskName, ::MmStrLen(kDiskName));
	::MmCopyMemory(block.Magic, (VoidPtr)kEPMMagic86, ::MmStrLen(kEPMMagic86));

	::uuid_generate_random((NeOS::UInt8*)&block.Guid);

	IOObject handle = IoOpenFile(kDiskName, nullptr);
	::IoWriteFile(handle, (NeOS::Char*)&block, sizeof(struct ::EPM_PART_BLOCK));

	struct ::NEFS_ROOT_PARTITION_BLOCK rpb
	{
		0
	};

	::MmCopyMemory(rpb.PartitionName, (VoidPtr)kDiskName, ::MmStrLen(kDiskName));
	::MmCopyMemory(rpb.Ident, (VoidPtr)kNeFSIdent, ::MmStrLen(kNeFSIdent));

	rpb.Version	 = kNeFSVersionInteger;
	rpb.EpmBlock = kEPMBootBlockLba;

	rpb.StartCatalog = kNeFSCatalogStartAddress;
	rpb.CatalogCount = 0;

	rpb.DiskSize = kDiskSz;

	rpb.SectorSize	= kDiskSectorSz;
	rpb.SectorCount = rpb.DiskSize / rpb.SectorSize;

	rpb.FreeSectors = rpb.SectorCount;
	rpb.FreeCatalog = rpb.DiskSize / sizeof(NEFS_CATALOG_STRUCT);

	auto p_prev = ::IoTellFile(handle);

	::IoWriteFile(handle, (NeOS::Char*)&rpb, sizeof(struct ::NEFS_ROOT_PARTITION_BLOCK));

	::IoSeekFile(handle, p_prev);

	::IoCloseFile(handle);

	return 0;
}