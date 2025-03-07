/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: NeFS.h
	PURPOSE: NeFS (New extended File System) support.

   ------------------------------------------- */

#include <iostream>
#include <fstream>
#include <FirmwareKit/EPM.h>
#include <FSKit/NeFS.h>
#include <uuid/uuid.h>

static const char* kDiskName = "Disk";
static const int kDiskSectorSz = 512;
static const int kDiskBlockCnt = 1;

/// @brief Filesystem tool entrypoint.
int main(int argc, char** argv)
{
    std::cout << "make_fs: EPM image creator.\n";
    
    struct EPM_PART_BLOCK block{0};

    block.NumBlocks = kDiskBlockCnt;
    block.SectorSz = kDiskSectorSz;
    block.Version = kEPMRevisionBcd;
    block.LbaStart = sizeof(struct EPM_PART_BLOCK);
    block.LbaEnd = 0;
    block.FsVersion = kNeFSVersionInteger;
   
    ::memcpy(block.Name, kDiskName, strlen(kDiskName));
    ::memcpy(block.Magic, kEPMMagic86, strlen(kEPMMagic86));

    ::uuid_generate_random((NeOS::UInt8*)&block.Guid);

    std::ofstream output_epm("disk.eimg");
    output_epm.write((NeOS::Char*)&block, sizeof(struct EPM_PART_BLOCK));
    
    output_epm.close();

    std::cout << "make_fs: EPM image has been written to disk.eimg.\n";

    return 0;
}