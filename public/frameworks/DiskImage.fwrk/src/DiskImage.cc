/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.cc
	PURPOSE: Disk Imaging framework.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>

/// @brief EPM format disk
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatDisk(struct DI_DISK_IMAGE& img) noexcept
{
	const char* disk_name = img.disk_name;
	int			sector_sz = img.sector_sz;
	const int	block_cnt = img.block_cnt;
	size_t		disk_sz	  = img.disk_sz;
	const char* out		  = img.out_name;

	if (!img.sector_sz || (img.sector_sz % 512 != 0))
		return kDIFailureStatus;

	if (*out == 0 ||
		*disk_name == 0)
		return kDIFailureStatus;

	struct ::EPM_PART_BLOCK block
	{
	};

	block.NumBlocks = block_cnt;
	block.SectorSz	= sector_sz;
	block.Version	= kEPMRevisionBcd;
	block.LbaStart	= sizeof(struct ::EPM_PART_BLOCK);
	block.LbaEnd	= disk_sz - block.LbaStart;
	block.FsVersion = kNeFSVersionInteger;

	::MmCopyMemory(block.Name, (VoidPtr)disk_name, ::MmStrLen(disk_name));
	::MmCopyMemory(block.Magic, (VoidPtr)kEPMMagic86, ::MmStrLen(kEPMMagic86));

	::uuid_generate_random((NeOS::UInt8*)&block.Guid);

	IOObject handle = IoOpenFile(out, nullptr);

	if (!handle)
		return kDIFailureStatus;

	::IoWriteFile(handle, (NeOS::Char*)&block, sizeof(struct ::EPM_PART_BLOCK));

	::IoCloseFile(handle);

	handle = nullptr;

	return kDISuccessStatus;
}

/// @brief NeFS format over EPM.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatNeFS(struct DI_DISK_IMAGE& img) noexcept
{
	const char* disk_name = img.disk_name;
	int			sector_sz = img.sector_sz;
	const int	block_cnt = img.block_cnt;
	size_t		disk_sz	  = img.disk_sz;
	const char* out		  = img.out_name;

	if (!img.sector_sz || (img.sector_sz % 512 != 0))
		return kDIFailureStatus;

	if (*out == 0 ||
		*disk_name == 0)
		return kDIFailureStatus;

	struct ::NEFS_ROOT_PARTITION_BLOCK rpb
	{
	};

	::MmCopyMemory(rpb.PartitionName, (VoidPtr)disk_name, ::MmStrLen(disk_name));
	::MmCopyMemory(rpb.Ident, (VoidPtr)kNeFSIdent, ::MmStrLen(kNeFSIdent));

	rpb.Version	 = kNeFSVersionInteger;
	rpb.EpmBlock = kEPMBootBlockLba;

	rpb.StartCatalog = kNeFSCatalogStartAddress;
	rpb.CatalogCount = 0;

	rpb.DiskSize = disk_sz;

	rpb.SectorSize	= sector_sz;
	rpb.SectorCount = rpb.DiskSize / rpb.SectorSize;

	rpb.FreeSectors = rpb.SectorCount;
	rpb.FreeCatalog = rpb.DiskSize / sizeof(NEFS_CATALOG_STRUCT);

	IOObject handle = IoOpenFile(out, nullptr);

	if (!handle)
		return kDIFailureStatus;

	UInt64 p_prev = ::IoTellFile(handle);

	::IoWriteFile(handle, (NeOS::Char*)&rpb, sizeof(struct ::NEFS_ROOT_PARTITION_BLOCK));

	::IoSeekFile(handle, p_prev);

	::IoCloseFile(handle);

	handle = nullptr;

	return kDISuccessStatus;
}