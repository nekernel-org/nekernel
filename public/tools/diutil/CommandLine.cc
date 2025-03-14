/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: NeFS.h
	PURPOSE: NeFS (New extended File System) support.

   ------------------------------------------- */

#include <string>
#include <iostream>
#include <fstream>

#include <FirmwareKit/EPM.h>
#include <FSKit/NeFS.h>
#include <uuid/uuid.h>

static std::string kDiskName	 = "Disk";
static int		   kDiskSectorSz = 512;
static const int   kDiskBlockCnt = 1;
static size_t	   kDiskSz		 = gib_cast(4);
static std::string kOutDisk		 = "disk.eimg";

/// @brief Filesystem tool entrypoint.
int main(int argc, char** argv)
{
	for (size_t arg = 0; arg < argc; ++arg)
	{
		std::string arg_s = argv[arg];

		if (arg_s == "--disk-output-name")
		{
			if ((arg + 1) < argc)
			{
				kOutDisk = argv[arg + 1];
			}
		}
		else if (arg_s == "--disk-output-size")
		{
			if ((arg + 1) < argc)
			{
				kDiskSz = strtol(argv[arg + 1], nullptr, 10);
			}
		}
		else if (arg_s == "--disk-sector-size")
		{
			if ((arg + 1) < argc)
			{
				kDiskSectorSz = strtol(argv[arg + 1], nullptr, 10);
			}
		}
		else if (arg_s == "--disk-part-name")
		{
			if ((arg + 1) < argc)
			{
				kDiskName = argv[arg + 1];
			}
		}
	}

	std::cout << "diutil: EPM Disk Tool.\n";

	struct ::EPM_PART_BLOCK block
	{
		0
	};

	block.NumBlocks = kDiskBlockCnt;
	block.SectorSz	= kDiskSectorSz;
	block.Version	= kEPMRevisionBcd;
	block.LbaStart	= sizeof(struct ::EPM_PART_BLOCK);
	block.LbaEnd	= kDiskSz - sizeof(struct ::EPM_PART_BLOCK);
	block.FsVersion = kNeFSVersionInteger;

	::memcpy(block.Name, kDiskName.c_str(), strlen(kDiskName.c_str()));
	::memcpy(block.Magic, kEPMMagic86, strlen(kEPMMagic86));

	::uuid_generate_random((NeOS::UInt8*)&block.Guid);

	uuid_string_t str;

	::uuid_unparse((NeOS::UInt8*)&block.Guid, str);

	std::cout << "diutil: Partition UUID: " << str << std::endl;

	std::ofstream output_epm(kOutDisk);
	output_epm.write((NeOS::Char*)&block, sizeof(struct ::EPM_PART_BLOCK));

	struct ::NEFS_ROOT_PARTITION_BLOCK rpb
	{
		0
	};

	::memcpy(rpb.PartitionName, kDiskName.c_str(), strlen(kDiskName.c_str()));
	::memcpy(rpb.Ident, kNeFSIdent, strlen(kNeFSIdent));

	rpb.Version	 = kNeFSVersionInteger;
	rpb.EpmBlock = kEPMBootBlockLba;

	rpb.StartCatalog = kNeFSCatalogStartAddress;
	rpb.CatalogCount = 0;

	rpb.DiskSize = kDiskSz;

	rpb.SectorSize	= kDiskSectorSz;
	rpb.SectorCount = rpb.DiskSize / rpb.SectorSize;

	rpb.FreeSectors = rpb.SectorCount;
	rpb.FreeCatalog = rpb.DiskSize / sizeof(NEFS_CATALOG_STRUCT);

	auto p_prev = output_epm.tellp();

	output_epm.seekp(kNeFSRootCatalogStartAddress);
	output_epm.write((NeOS::Char*)&rpb, sizeof(struct ::NEFS_ROOT_PARTITION_BLOCK));

	output_epm.seekp(p_prev);
	output_epm.close();

	std::cout << "diutil: EPM Disk has been written to: " << kOutDisk << "\n";

	return 0;
}