/* -------------------------------------------

	Copyright (C) 2024, ELMH GROUP, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_NEFS__

#include <FSKit/NeFS.h>
#include <FirmwareKit/EPM.h>

#include <Modules/AHCI/AHCI.h>
#include <Modules/ATA/ATA.h>
#include <Modules/Flash/Flash.h>
#include <KernelKit/LPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/Stop.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/User.h>

using namespace Kernel;

#ifdef __ZKA_NO_BUILTIN__
/***********************************************************************************/
/**
	Define those external symbols, to make the editor shutup
*/
/***********************************************************************************/

/***********************************************************************************/
/// @brief get sector count.
/***********************************************************************************/
Kernel::SizeT drv_get_sector_count();

/***********************************************************************************/
/// @brief get device size.
/***********************************************************************************/
Kernel::SizeT drv_get_size();

#endif

///! BUGS: 0

/***********************************************************************************/
/// This file implements the New File System.
///	New File System implements a B-Tree based algortihm.
///		/
///	/Path1/		/ath2/
/// /readme.rtf		/ListContents.pef /readme.lnk <-- symlink.
///								/Path1/readme.rtf
/***********************************************************************************/

STATIC MountpointInterface kDiskMountpoint;

/***********************************************************************************/
/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param the_fork the fork itself.
/// @return the fork
/***********************************************************************************/
_Output NFS_FORK_STRUCT* NeFSParser::CreateFork(_Input NFS_CATALOG_STRUCT* catalog,
												_Input NFS_FORK_STRUCT& the_fork)
{
	if (catalog && the_fork.ForkName[0] != 0 &&
		the_fork.DataSize <= kNeFSForkDataSz)
	{
		Lba lba = (the_fork.Kind == kNeFSDataForkKind) ? catalog->DataFork
													   : catalog->ResourceFork;

		kcout << "Fork LBA: " << hex_number(lba) << endl;

		if (lba <= kNeFSCatalogStartAddress)
			return nullptr;

		auto drv = kDiskMountpoint.A();

		/// special treatment.
		rt_copy_memory((VoidPtr) "fs/nefs-packet", drv.fPacket.fPacketMime,
					   rt_string_len("fs/nefs-packet"));

		NFS_FORK_STRUCT curFork{0};
		NFS_FORK_STRUCT prevFork{0};
		Lba				lbaOfPreviousFork = lba;

		/// do not check for anything. Loop until we get what we want, that is a free fork zone.
		while (true)
		{
			if (lba <= kNeFSCatalogStartAddress)
				break;

			drv.fPacket.fPacketLba	   = lba;
			drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
			drv.fPacket.fPacketContent = &curFork;

			drv.fInput(&drv.fPacket);

			if (curFork.NextSibling > kBadAddress)
			{
				kcout << "Bad fork: " << hex_number(curFork.NextSibling) << endl;
				break;
			}

			kcout << "Next fork: " << hex_number(curFork.NextSibling) << endl;

			if (curFork.Flags & kNeFSFlagCreated)
			{
				kcout << "Fork already exists.\r";

				/// sanity check.
				if (StringBuilder::Equals(curFork.ForkName, the_fork.ForkName) &&
					StringBuilder::Equals(curFork.CatalogName, catalog->Name))
					return nullptr;

				kcout << "Next fork: " << hex_number(curFork.NextSibling) << endl;

				lbaOfPreviousFork = lba;
				lba				  = curFork.NextSibling;

				prevFork = curFork;
			}
			else
			{
				/// This is a check that we have, in order to link the previous fork
				/// entry.
				if (lba >= kNeFSCatalogStartAddress)
				{
					drv.fPacket.fPacketLba	   = lbaOfPreviousFork;
					drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
					drv.fPacket.fPacketContent = &prevFork;

					prevFork.NextSibling = lba;

					/// write to disk.
					drv.fOutput(&drv.fPacket);
				}

				break;
			}
		}

		the_fork.Flags |= kNeFSFlagCreated;
		the_fork.DataOffset		 = lba - sizeof(NFS_FORK_STRUCT);
		the_fork.PreviousSibling = lbaOfPreviousFork;
		the_fork.NextSibling	 = the_fork.DataOffset - the_fork.DataSize - sizeof(NFS_FORK_STRUCT);

		drv.fPacket.fPacketLba	   = lba;
		drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
		drv.fPacket.fPacketContent = &the_fork;

		drv.fOutput(&drv.fPacket);

		/// log what we have now.
		kcout << "Wrote fork data at: " << hex_number(the_fork.DataOffset)
			  << endl;

		kcout << "Wrote fork at: " << hex_number(lba) << endl;

		return &the_fork;
	}

	return nullptr;
}

/***********************************************************************************/
/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
/***********************************************************************************/
_Output NFS_FORK_STRUCT* NeFSParser::FindFork(_Input NFS_CATALOG_STRUCT* catalog,
											  _Input const Char* name,
											  Boolean			 isDataFork)
{
	auto			 drv	  = kDiskMountpoint.A();
	NFS_FORK_STRUCT* the_fork = nullptr;

	Lba lba = isDataFork ? catalog->DataFork : catalog->ResourceFork;

	while (lba != 0)
	{
		drv.fPacket.fPacketLba	   = lba;
		drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
		drv.fPacket.fPacketContent = (VoidPtr)the_fork;

		rt_copy_memory((VoidPtr) "fs/nefs-packet", drv.fPacket.fPacketMime, 16);

		if (auto res =
				fs_newfs_read(&kDiskMountpoint, drv, this->fDriveIndex);
			res)
		{
			switch (res)
			{
			case 1:
				err_local_get() = kErrorDiskReadOnly;
				break;
			case 2:
				err_local_get() = kErrorDiskIsFull;
				break;
				err_local_get() = kErrorNoSuchDisk;
				break;

			default:
				break;
			}
			return nullptr;
		}

		if (StringBuilder::Equals(the_fork->ForkName, name))
		{
			break;
		}

		lba = the_fork->NextSibling;
	}

	return the_fork;
}

/***********************************************************************************/
/// @brief Simpler factory to create a catalog (assumes you want to create a
/// file.)
/// @param name
/// @return catalog pointer.
/***********************************************************************************/
_Output NFS_CATALOG_STRUCT* NeFSParser::CreateCatalog(_Input const Char* name)
{
	return this->CreateCatalog(name, 0, kNeFSCatalogKindFile);
}

/***********************************************************************************/
/// @brief Creates a new catalog into the disk.
/// @param name the catalog name.
/// @param flags the flags of the catalog.
/// @param kind the catalog kind.
/// @return catalog pointer.
/***********************************************************************************/
_Output NFS_CATALOG_STRUCT* NeFSParser::CreateCatalog(_Input const Char* name,
													  _Input const Int32& flags,
													  _Input const Int32& kind)
{
	kcout << "CreateCatalog(...)\r";

	Lba out_lba = 0UL;

	kcout << "Checking for path separator...\r";

	/// a directory should have a slash in the end.
	if (kind == kNeFSCatalogKindDir &&
		name[rt_string_len(name) - 1] != NeFileSystemHelper::Separator())
		return nullptr;

	/// a file shouldn't have a slash in the end.
	if (kind != kNeFSCatalogKindDir &&
		name[rt_string_len(name) - 1] == NeFileSystemHelper::Separator())
		return nullptr;

	NFS_CATALOG_STRUCT* catalog_copy = this->FindCatalog(name, out_lba);

	if (catalog_copy)
	{
		kcout << "Catalog already exists: " << name << ".\r";
		err_local_get() = kErrorFileExists;

		return catalog_copy;
	}

	Char parentName[kNeFSNodeNameLen] = {0};

	for (SizeT indexName = 0UL; indexName < rt_string_len(name); ++indexName)
	{
		parentName[indexName] = name[indexName];
	}

	if (*parentName == 0)
	{
		kcout << "Parent name is NUL.\r";
		err_local_get() = kErrorFileNotFound;
		return nullptr;
	}

	/// Locate parent catalog, to then allocate right after it.

	for (SizeT indexFill = 0; indexFill < rt_string_len(name); ++indexFill)
	{
		parentName[indexFill] = name[indexFill];
	}

	SizeT indexReverseCopy = rt_string_len(parentName);

	// zero character it.
	parentName[--indexReverseCopy] = 0;

	// mandatory / character, zero it.
	parentName[--indexReverseCopy] = 0;

	while (parentName[indexReverseCopy] != NeFileSystemHelper::Separator())
	{
		parentName[indexReverseCopy] = 0;
		--indexReverseCopy;
	}

	NFS_CATALOG_STRUCT* catalog = this->FindCatalog(parentName, out_lba);

	auto drive = kDiskMountpoint.A();

	if (catalog && catalog->Kind == kNeFSCatalogKindFile)
	{
		kcout << "Parent name is file.\r";
		delete catalog;
		return nullptr;
	}
	else if (!catalog)
	{
		Char sectorBufPartBlock[kNeFSSectorSz] = {0};

		drive.fPacket.fPacketContent = sectorBufPartBlock;
		drive.fPacket.fPacketSize	 = kNeFSSectorSz;
		drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

		drive.fInput(&drive.fPacket);

		constexpr auto cNeFSCatalogPadding = 4;

		NFS_ROOT_PARTITION_BLOCK* part_block = (NFS_ROOT_PARTITION_BLOCK*)sectorBufPartBlock;
		out_lba								 = part_block->StartCatalog;
	}

	constexpr SizeT kDefaultForkSize = kNeFSForkSize;

	NFS_CATALOG_STRUCT* child_catalog = new NFS_CATALOG_STRUCT();

	Int32 flagsList = flags;

	child_catalog->ResourceForkSize = kDefaultForkSize;
	child_catalog->DataForkSize		= kDefaultForkSize;

	child_catalog->NextSibling = out_lba;
	child_catalog->PrevSibling = out_lba;
	child_catalog->Kind		   = kind;
	child_catalog->Flags	   = kNeFSFlagCreated | flagsList;

	rt_copy_memory((VoidPtr)name, (VoidPtr)child_catalog->Name,
				   rt_string_len(name));

	NFS_CATALOG_STRUCT temporary_catalog;

	Lba start_free = out_lba;

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	drive.fPacket.fPacketContent = &temporary_catalog;
	drive.fPacket.fPacketSize	 = kNeFSSectorSz;
	drive.fPacket.fPacketLba	 = start_free;

	drive.fInput(&drive.fPacket);

	NFS_CATALOG_STRUCT* next_sibling = reinterpret_cast<NFS_CATALOG_STRUCT*>(&temporary_catalog);

	start_free = next_sibling->NextSibling;

	child_catalog->PrevSibling = out_lba;

	drive.fPacket.fPacketLba = start_free;
	drive.fInput(&drive.fPacket);

	while (drive.fPacket.fPacketGood)
	{
		next_sibling = reinterpret_cast<NFS_CATALOG_STRUCT*>(&temporary_catalog);

		if (start_free <= kNeFSRootCatalogStartAddress)
		{
			delete child_catalog;
			delete catalog;

			return nullptr;
		}

		// ========================== //
		// Allocate catalog now...
		// ========================== //
		if ((next_sibling->Flags & kNeFSFlagCreated) == 0)
		{
			Char sectorBufPartBlock[kNeFSSectorSz] = {0};

			drive.fPacket.fPacketContent = sectorBufPartBlock;
			drive.fPacket.fPacketSize	 = kNeFSSectorSz;
			drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

			drive.fInput(&drive.fPacket);

			constexpr auto cNeFSCatalogPadding = 4;

			NFS_ROOT_PARTITION_BLOCK* part_block = (NFS_ROOT_PARTITION_BLOCK*)sectorBufPartBlock;

			if (part_block->FreeCatalog < 1)
			{
				delete child_catalog;
				return nullptr;
			}

			child_catalog->DataFork		= part_block->DiskSize - start_free;
			child_catalog->ResourceFork = child_catalog->DataFork;

			// Write the new catalog next sibling, if we don't know this parent. //
			// This is necessary, so that we don't have to get another lba to allocate. //
			if (!StringBuilder::Equals(parentName, next_sibling->Name))
			{
				child_catalog->NextSibling =
					start_free + (sizeof(NFS_CATALOG_STRUCT) * cNeFSCatalogPadding);
			}

			drive.fPacket.fPacketContent = child_catalog;
			drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
			drive.fPacket.fPacketLba	 = start_free;

			drive.fOutput(&drive.fPacket);

			// Get NeFS partition's block.

			drive.fPacket.fPacketContent = sectorBufPartBlock;
			drive.fPacket.fPacketSize	 = kNeFSSectorSz;
			drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

			drive.fInput(&drive.fPacket);

			part_block->FreeSectors -= 1;
			part_block->CatalogCount += 1;
			part_block->FreeCatalog -= 1;

			drive.fOutput(&drive.fPacket);

			kcout << "Create new catalog, status: "
				  << hex_number(child_catalog->Flags) << endl;
			kcout << "Create new catalog, name: " << child_catalog->Name
				  << endl;

			delete catalog;
			return child_catalog;
		}
		else if ((next_sibling->Flags & kNeFSFlagCreated) &&
				 StringBuilder::Equals(next_sibling->Name, name))
		{
			return next_sibling;
		}

		constexpr auto cNeFSCatalogPadding = 4;

		//// @note that's how we find the next catalog in the partition block.
		start_free = start_free + (sizeof(NFS_CATALOG_STRUCT) * cNeFSCatalogPadding);

		drive.fPacket.fPacketContent = &temporary_catalog;
		drive.fPacket.fPacketSize	 = kNeFSSectorSz;
		drive.fPacket.fPacketLba	 = start_free;

		drive.fInput(&drive.fPacket);
	}

	delete catalog;
	return nullptr;
}

/// @brief Make a EPM+NeFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see err_local_get().
bool NeFSParser::Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name)
{
	if (*part_name == 0 ||
		endLba == 0)
		return false;

	// verify disk.
	drive->fVerify(&drive->fPacket);

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	// if disk isn't good, then error out.
	if (false == drive->fPacket.fPacketGood)
	{
		err_local_get() = kErrorDiskIsCorrupted;
		return false;
	}

	Char fs_buf[kNeFSSectorSz] = {0};

	Lba start = kNeFSRootCatalogStartAddress;

	drive->fPacket.fPacketContent = fs_buf;
	drive->fPacket.fPacketSize	  = kNeFSSectorSz;
	drive->fPacket.fPacketLba	  = start;

	drive->fInput(&drive->fPacket);

	if (flags & kNeFSPartitionTypeBoot)
	{
		// make it bootable when needed.
		Char bufEpmHdr[kNeFSSectorSz] = {0};

		BOOT_BLOCK_STRUCT* epmBoot = (BOOT_BLOCK_STRUCT*)bufEpmHdr;

		// Write a new EPM entry.

		constexpr auto kFsName	  = "NeFS";
		constexpr auto kBlockName = "ZKA:";

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kFsName)), epmBoot->Fs, rt_string_len(kFsName));

		epmBoot->FsVersion = kNeFSVersionInteger;
		epmBoot->LbaStart  = start;
		epmBoot->SectorSz  = kNeFSSectorSz;

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), epmBoot->Name, rt_string_len(kBlockName));
		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), epmBoot->Magic, rt_string_len(kEPMMagic));

		Lba outEpmLba = kEPMBaseLba;

		Char buf[kNeFSSectorSz];

		Lba	  prevStart = 0;
		SizeT cnt		= 0;

		while (drive->fPacket.fPacketGood)
		{
			drive->fPacket.fPacketContent = buf;
			drive->fPacket.fPacketSize	  = kNeFSSectorSz;
			drive->fPacket.fPacketLba	  = outEpmLba;

			drive->fInput(&drive->fPacket);

			if (buf[0] == 0)
			{
				epmBoot->LbaStart = prevStart;

				if (epmBoot->LbaStart)
					epmBoot->LbaStart = outEpmLba;

				epmBoot->LbaEnd	   = endLba;
				epmBoot->NumBlocks = cnt;

				drive->fPacket.fPacketContent = bufEpmHdr;
				drive->fPacket.fPacketSize	  = kNeFSSectorSz;
				drive->fPacket.fPacketLba	  = outEpmLba;

				drive->fOutput(&drive->fPacket);

				break;
			}
			else
			{
				prevStart = ((BOOT_BLOCK_STRUCT*)buf)->LbaStart + ((BOOT_BLOCK_STRUCT*)buf)->LbaEnd;
			}

			outEpmLba += sizeof(BOOT_BLOCK_STRUCT);
			++cnt;
		}
	}

	// disk isnt faulty and data has been fetched.
	while (drive->fPacket.fPacketGood)
	{
		NFS_ROOT_PARTITION_BLOCK* part_block = (NFS_ROOT_PARTITION_BLOCK*)fs_buf;

		// check for an empty partition here.
		if (part_block->PartitionName[0] == 0 &&
			rt_string_cmp(part_block->Ident, kNeFSIdent, kNeFSIdentLen))
		{
			// partition is free and valid.

			part_block->Version = kNeFSVersionInteger;

			const auto cUntitledHD = part_name;

			rt_copy_memory((VoidPtr)kNeFSIdent, (VoidPtr)part_block->Ident,
						   kNeFSIdentLen);

			rt_copy_memory((VoidPtr)cUntitledHD, (VoidPtr)part_block->PartitionName,
						   rt_string_len(cUntitledHD));

			SizeT catalogCount = 0UL;

			SizeT sectorCount = drv_get_sector_count();
			SizeT diskSize	  = drv_get_size();

			part_block->Kind		 = kNeFSPartitionTypeStandard;
			part_block->StartCatalog = kNeFSCatalogStartAddress;
			part_block->Flags		 = kNeFSPartitionTypeStandard;
			part_block->CatalogCount = sectorCount / sizeof(NFS_CATALOG_STRUCT);
			part_block->SectorCount	 = sectorCount;
			part_block->DiskSize	 = diskSize;
			part_block->FreeCatalog	 = sectorCount / sizeof(NFS_CATALOG_STRUCT);

			drive->fPacket.fPacketContent = fs_buf;
			drive->fPacket.fPacketSize	  = kNeFSSectorSz;
			drive->fPacket.fPacketLba	  = kNeFSRootCatalogStartAddress;

			drive->fOutput(&drive->fPacket);

			kcout << "drive kind: " << drive->fDriveKind() << endl;

			kcout << "partition name: " << part_block->PartitionName << endl;
			kcout << "start: " << hex_number(part_block->StartCatalog) << endl;
			kcout << "number of catalogs: " << hex_number(part_block->CatalogCount) << endl;
			kcout << "free catalog: " << hex_number(part_block->FreeCatalog) << endl;
			kcout << "free sectors: " << hex_number(part_block->FreeSectors) << endl;
			kcout << "sector size: " << hex_number(part_block->SectorSize) << endl;

			// write the root catalog.
			this->CreateCatalog(kNeFSRoot, 0, kNeFSCatalogKindDir);

			return true;
		}

		kcout << "partition block already exists.\r";

		start += part_block->DiskSize;

		drive->fPacket.fPacketContent = fs_buf;
		drive->fPacket.fPacketSize	  = kNeFSSectorSz;
		drive->fPacket.fPacketLba	  = start;

		drive->fInput(&drive->fPacket);
	}

	return false;
}

/// @brief Writes the data fork into a specific catalog.
/// @param catalog the catalog itself
/// @param data the data.
/// @return if the catalog w rote the contents successfully.
bool NeFSParser::WriteCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog, Bool is_rsrc_fork, _Input VoidPtr data, _Input SizeT size_of_data, _Input const Char* forkName)
{
	if (size_of_data > kNeFSForkDataSz ||
		size_of_data == 0)
		return No;

	auto buf = new UInt8[kNeFSForkDataSz];
	rt_set_memory(buf, 0, kNeFSForkDataSz);

	rt_copy_memory(data, buf, size_of_data);

	auto drive = kDiskMountpoint.A();

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	auto startFork = (!is_rsrc_fork) ? catalog->DataFork
									 : catalog->ResourceFork;

	NFS_FORK_STRUCT* fork_data_input = new NFS_FORK_STRUCT();
	NFS_FORK_STRUCT	 prevFork{};

	// sanity check of the fork position as the condition to run the loop.
	while (startFork >= kNeFSCatalogStartAddress)
	{
		drive.fPacket.fPacketContent = fork_data_input;
		drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
		drive.fPacket.fPacketLba	 = startFork;

		drive.fInput(&drive.fPacket);

		// check the fork, if it's position is valid.
		if (fork_data_input->DataOffset <= kNeFSCatalogStartAddress)
		{
			err_local_get() = kErrorDiskIsCorrupted;

			kcout << "Invalid fork offset.\r";

			return false;
		}

		if (fork_data_input->Flags != kNeFSFlagUnallocated &&
			fork_data_input->Flags != kNeFSFlagDeleted &&
			StringBuilder::Equals(fork_data_input->ForkName, forkName) &&
			StringBuilder::Equals(fork_data_input->CatalogName, catalog->Name) &&
			fork_data_input->DataSize == size_of_data)
		{
			// ===================================================== //
			// Store the blob now.
			// ===================================================== //

			fork_data_input->Flags |= kNeFSFlagCreated;

			drive.fPacket.fPacketContent = buf;
			drive.fPacket.fPacketSize	 = kNeFSForkDataSz;
			drive.fPacket.fPacketLba	 = fork_data_input->DataOffset;

			kcout << "data offset: " << hex_number(fork_data_input->DataOffset) << endl;

			drive.fOutput(&drive.fPacket);

			drive.fPacket.fPacketContent = fork_data_input;
			drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
			drive.fPacket.fPacketLba	 = startFork - sizeof(NFS_FORK_STRUCT);

			drive.fOutput(&drive.fPacket);

			kcout << "wrote fork at offset: " << hex_number(fork_data_input->DataOffset) << endl;
			kcout << "wrote fork at offset: " << hex_number(startFork - sizeof(NFS_FORK_STRUCT)) << endl;

			delete catalog;

			return true;
		}

		// stumble upon a fork, store it.

		prevFork = *fork_data_input;

		startFork = fork_data_input->NextSibling;
	}

	return false;
}

/// @brief
/// @param catalogName the catalog name.
/// @return the newly found catalog.
_Output NFS_CATALOG_STRUCT* NeFSParser::FindCatalog(_Input const Char* catalogName,
													Lba&			   out_lba)
{
	kcout << "Start finding catalog...\r";

	NFS_ROOT_PARTITION_BLOCK fs_buf{0};
	auto					 drive = kDiskMountpoint.A();

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	drive.fPacket.fPacketContent = &fs_buf;
	drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
	drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

	drive.fInput(&drive.fPacket);

	NFS_ROOT_PARTITION_BLOCK* part = (NFS_ROOT_PARTITION_BLOCK*)&fs_buf;

	auto	   startCatalogList	 = part->StartCatalog;
	const auto cCtartCatalogList = startCatalogList;

	auto localSearchFirst = false;

	NFS_CATALOG_STRUCT temporary_catalog{0};

	drive.fPacket.fPacketLba	 = startCatalogList;
	drive.fPacket.fPacketContent = &temporary_catalog;
	drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);

	drive.fInput(&drive.fPacket);

	if (!StringBuilder::Equals(catalogName, NeFileSystemHelper::Root()))
	{
		Char parentName[kNeFSNodeNameLen] = {0};

		for (SizeT indexFill = 0; indexFill < rt_string_len(catalogName); ++indexFill)
		{
			parentName[indexFill] = catalogName[indexFill];
		}

		SizeT indexReverseCopy = rt_string_len(parentName);

		// zero character.
		parentName[--indexReverseCopy] = 0;

		// mandatory '/' character.
		parentName[--indexReverseCopy] = 0;

		while (parentName[indexReverseCopy] != NeFileSystemHelper::Separator())
		{
			parentName[indexReverseCopy] = 0;
			--indexReverseCopy;
		}

		NFS_CATALOG_STRUCT* parentCatalog = this->FindCatalog(parentName, out_lba);

		if (parentCatalog &&
			!StringBuilder::Equals(parentName, NeFileSystemHelper::Root()))
		{
			startCatalogList = parentCatalog->NextSibling;
			delete parentCatalog;

			localSearchFirst = true;
		}
		else if (parentCatalog)
		{
			delete parentCatalog;
		}
		else
		{
			return nullptr;
		}
	}

	kcout << "Fetching catalog...\r";

NeFSSearchThroughCatalogList:
	while (drive.fPacket.fPacketGood)
	{
		drive.fPacket.fPacketLba	 = startCatalogList;
		drive.fPacket.fPacketContent = &temporary_catalog;
		drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);

		drive.fInput(&drive.fPacket);

		NFS_CATALOG_STRUCT* catalog = (NFS_CATALOG_STRUCT*)&temporary_catalog;

		if (StringBuilder::Equals(catalogName, catalog->Name))
		{
			/// ignore unallocated catalog, break
			if (!(catalog->Flags & kNeFSFlagCreated))
			{
				goto NeFSContinueSearch;
			}

			NFS_CATALOG_STRUCT* catalogPtr = new NFS_CATALOG_STRUCT();
			rt_copy_memory(catalog, catalogPtr, sizeof(NFS_CATALOG_STRUCT));

			kcout << "Found catalog at: " << hex_number(startCatalogList) << endl;
			kcout << "Found catalog at: " << catalog->Name << endl;

			out_lba = startCatalogList;
			return catalogPtr;
		}

	NeFSContinueSearch:
		startCatalogList = catalog->NextSibling;

		if (startCatalogList <= kNeFSRootCatalogStartAddress)
			break;
	}

	if (localSearchFirst)
	{
		localSearchFirst = false;
		startCatalogList = cCtartCatalogList;

		goto NeFSSearchThroughCatalogList;
	}

	out_lba = 0UL;
	return nullptr;
}

/// @brief Get catalog from filesystem.
/// @param name the catalog's name/
/// @return
_Output NFS_CATALOG_STRUCT* NeFSParser::GetCatalog(_Input const Char* name)
{
	Lba unused = 0;
	return this->FindCatalog(name, unused);
}

/// @brief Closes a catalog, (frees it).
/// @param catalog the catalog to close.
/// @return
Boolean NeFSParser::CloseCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog)
{
	if (!catalog)
		return false;

	delete catalog;
	catalog = nullptr;

	return true;
}

/// @brief Mark catalog as removed.
/// @param catalog The catalog structure.
/// @return if the catalog was removed or not.
Boolean NeFSParser::RemoveCatalog(_Input const Char* catalogName)
{
	if (!catalogName ||
		StringBuilder::Equals(catalogName, NeFileSystemHelper::Root()))
	{
		err_local_get() = kErrorInternal;
		return false;
	}

	Lba	 out_lba = 0;
	auto catalog = this->FindCatalog(catalogName, out_lba);

	if (out_lba >= kNeFSCatalogStartAddress ||
		catalog->Flags & kNeFSFlagCreated)
	{
		catalog->Flags |= kNeFSFlagDeleted;

		auto drive = kDiskMountpoint.A();

		rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
					   rt_string_len("fs/nefs-packet"));

		drive.fPacket.fPacketLba = out_lba; // the catalog position.
		drive.fPacket.fPacketSize =
			sizeof(NFS_CATALOG_STRUCT);			// size of catalog. roughly the sector size.
		drive.fPacket.fPacketContent = catalog; // the catalog itself.

		drive.fOutput(&drive.fPacket); // send packet.

		Char partitionBlockBuf[sizeof(NFS_ROOT_PARTITION_BLOCK)] = {0};

		drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;
		drive.fPacket.fPacketContent = partitionBlockBuf;
		drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);

		drive.fInput(&drive.fPacket);

		NFS_ROOT_PARTITION_BLOCK* part_block =
			reinterpret_cast<NFS_ROOT_PARTITION_BLOCK*>(partitionBlockBuf);

		--part_block->CatalogCount;
		++part_block->FreeSectors;

		drive.fOutput(&drive.fPacket);

		return true;
	}

	delete catalog;
	return false;
}

/// ***************************************************************** ///
/// Reading,Seek,Tell are unimplemented on catalogs, refer to forks I/O instead.
/// ***************************************************************** ///

/***********************************************************************************/
/// @brief Read the catalog data fork.
/// @param catalog
/// @param dataSz
/// @return
/***********************************************************************************/

VoidPtr NeFSParser::ReadCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
								_Input Bool						   is_rsrc_fork,
								_Input SizeT					   dataSz,
								_Input const Char* forkName)
{
	if (!catalog)
	{
		err_local_get() = kErrorFileNotFound;
		return nullptr;
	}

	constexpr auto cNeFSCatalogPadding = 4;

	Lba	 dataForkLba  = (!is_rsrc_fork) ? catalog->DataFork : catalog->ResourceFork;
	Size dataForkSize = (!is_rsrc_fork) ? catalog->DataForkSize : catalog->ResourceForkSize;

	kcout << "catalog " << catalog->Name
		  << ", fork: " << hex_number(dataForkLba) << endl;

	NFS_FORK_STRUCT* fs_buf = new NFS_FORK_STRUCT();
	auto			 drive	= kDiskMountpoint.A();

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	NFS_FORK_STRUCT* fs_fork_data = nullptr;

	while (dataForkLba > kNeFSCatalogStartAddress)
	{
		drive.fPacket.fPacketLba	 = dataForkLba;
		drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
		drive.fPacket.fPacketContent = fs_buf;

		drive.fInput(&drive.fPacket);

		fs_fork_data = fs_buf;

		kcout << "ForkName: " << fs_fork_data->ForkName << endl;
		kcout << "CatalogName: " << fs_fork_data->CatalogName << endl;

		if (StringBuilder::Equals(forkName, fs_fork_data->ForkName) &&
			StringBuilder::Equals(catalog->Name, fs_fork_data->CatalogName))
			break;

		dataForkLba = fs_fork_data->NextSibling;
	}

	if (dataForkLba < kNeFSCatalogStartAddress)
	{
		delete fs_buf;
		return nullptr;
	}

	return fs_fork_data;
}

/***********************************************************************************/
/// @brief Seek in the data fork.
/// @param catalog the catalog offset.
/// @param off where to seek.
/// @return if the seeking was successful.
/***********************************************************************************/

bool NeFSParser::Seek(_Input _Output NFS_CATALOG_STRUCT* catalog, SizeT off)
{
	if (!catalog)
	{
		err_local_get() = kErrorFileNotFound;
		return false;
	}

	err_local_get() = kErrorUnimplemented;
	return false;
}

/***********************************************************************************/
/// @brief Tell where we are inside the data fork.
/// @param catalog
/// @return The position on the file.
/***********************************************************************************/

SizeT NeFSParser::Tell(_Input _Output NFS_CATALOG_STRUCT* catalog)
{
	if (!catalog)
	{
		err_local_get() = kErrorFileNotFound;
		return 0;
	}

	err_local_get() = kErrorUnimplemented;
	return 0;
}

namespace Kernel::Detail
{
	/***********************************************************************************/
	/// @brief Construct NeFS drives.
	/***********************************************************************************/
	Boolean fs_init_newfs(Void) noexcept
	{
		kcout << "Creating A: drive...\r";
		kcout << "Creating A:\r";

		kDiskMountpoint.A() = io_construct_main_drive();
		kDiskMountpoint.B() = io_construct_blank_drive();
		kDiskMountpoint.C() = io_construct_blank_drive();
		kDiskMountpoint.D() = io_construct_blank_drive();

		kcout << "Creating A: [ OK ]\r";

		return true;
	}
} // namespace Kernel::Detail

#endif // ifdef __FSKIT_INCLUDES_NEFS__
