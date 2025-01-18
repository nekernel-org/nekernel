/* -------------------------------------------

	Copyright (C) 2024, MediaSwirl, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_NEFS__

#include <FSKit/NeFS.h>
#include <FirmwareKit/EPM.h>

#include <Mod/AHCI/AHCI.h>
#include <Mod/ATA/ATA.h>
#include <Mod/MFlash/MFlash.h>
#include <KernelKit/LPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/KernelPanic.h>
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
/// This file implements the New extended File System.
///	New extended File System implements a B-Tree based algortihm.
///		/
///	/Path1/		/ath2/
/// /readme.rtf		/ListContents.pef /readme.lnk <-- symlink.
///								/Path1/readme.rtf
/***********************************************************************************/

STATIC MountpointInterface kMountpoint;

/***********************************************************************************/
/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param the_fork the fork itself.
/// @return the fork
/***********************************************************************************/
_Output BOOL NeFileSystemParser::CreateFork(_Input NFS_FORK_STRUCT& the_fork)
{
	if (the_fork.ForkName[0] != 0 &&
		the_fork.CatalogName[0] != 0 &&
		the_fork.DataSize > 0)
	{
		auto catalog = this->GetCatalog(the_fork.CatalogName);

		if (!catalog)
			return NO;

		Lba lba = catalog->DataFork;

		kcout << "Fork LBA: " << hex_number(lba) << endl;

		if (lba < kNeFSCatalogStartAddress)
			return NO;

		auto drv = kMountpoint.A();

		Lba lbaOfPreviousFork = lba;

		NFS_FORK_STRUCT prevFork;

		/// do not check for anything. Loop until we get what we want, that is a free fork zone.
		while (drv.fPacket.fPacketGood)
		{
			NFS_FORK_STRUCT curFork;

			drv.fPacket.fPacketLba	   = lba;
			drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
			drv.fPacket.fPacketContent = &curFork;

			drv.fInput(&drv.fPacket);

			kcout << "Next fork: " << hex_number(curFork.NextSibling) << endl;

			if (curFork.Flags & kNeFSFlagCreated)
			{
				kcout << "Fork already exists.\r";

				/// sanity check.
				if (StringBuilder::Equals(curFork.ForkName, the_fork.ForkName) &&
					StringBuilder::Equals(curFork.CatalogName, the_fork.CatalogName))
					break;

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
		the_fork.NextSibling	 = (the_fork.DataOffset - the_fork.DataSize - sizeof(NFS_FORK_STRUCT));

		drv.fPacket.fPacketLba	   = lba;
		drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
		drv.fPacket.fPacketContent = &the_fork;

		kcout << "Writing fork...\r";

		drv.fOutput(&drv.fPacket);

		/// log what we have now.
		kcout << "Wrote fork data at: " << hex_number(the_fork.DataOffset)
			  << endl;

		kcout << "Wrote fork at: " << hex_number(lba) << endl;

		return YES;
	}

	return NO;
}

/***********************************************************************************/
/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
/***********************************************************************************/
_Output NFS_FORK_STRUCT* NeFileSystemParser::FindFork(_Input NFS_CATALOG_STRUCT* catalog,
													  _Input const Char*		 name,
													  Boolean					 isDataFork)
{
	auto			 drive	  = kMountpoint.A();
	NFS_FORK_STRUCT* the_fork = nullptr;

	Lba lba = isDataFork ? catalog->DataFork : catalog->ResourceFork;

	while (lba != 0)
	{
		drive.fPacket.fPacketLba	 = lba;
		drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
		drive.fPacket.fPacketContent = (VoidPtr)the_fork;

		rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime, 16);

		if (auto res =
				fs_nefs_read(&kMountpoint, drive, this->mDriveIndex);
			res)
		{
			switch (res)
			{
			case 1:
				err_global_get() = kErrorDiskReadOnly;
				break;
			case 2:
				err_global_get() = kErrorDiskIsFull;
				break;
				err_global_get() = kErrorNoSuchDisk;
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
_Output NFS_CATALOG_STRUCT* NeFileSystemParser::CreateCatalog(_Input const Char* name)
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
_Output NFS_CATALOG_STRUCT* NeFileSystemParser::CreateCatalog(_Input const Char*  name,
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
		err_global_get() = kErrorFileExists;

		delete catalog_copy;
		catalog_copy = nullptr;

		return nullptr;
	}

	Char parent_name[kNeFSNodeNameLen] = {0};

	for (SizeT indexName = 0UL; indexName < rt_string_len(name); ++indexName)
	{
		parent_name[indexName] = name[indexName];
	}

	if (*parent_name == 0)
	{
		kcout << "Parent name is NUL.\r";
		err_global_get() = kErrorFileNotFound;
		return nullptr;
	}

	/// Locate parent catalog, to then allocate right after it.

	for (SizeT index_fill = 0; index_fill < rt_string_len(name); ++index_fill)
	{
		parent_name[index_fill] = name[index_fill];
	}

	SizeT index_reverse_copy = rt_string_len(parent_name);

	// zero character it.
	parent_name[--index_reverse_copy] = 0;

	// mandatory / character, zero it.
	parent_name[--index_reverse_copy] = 0;

	while (parent_name[index_reverse_copy] != NeFileSystemHelper::Separator())
	{
		parent_name[index_reverse_copy] = 0;
		--index_reverse_copy;
	}

	NFS_CATALOG_STRUCT* catalog = this->FindCatalog(parent_name, out_lba);

	auto drive = kMountpoint.A();

	constexpr auto kNeFSCatalogPadding = 4;

	if (catalog && catalog->Kind == kNeFSCatalogKindFile)
	{
		kcout << "Parent is a file.\r";
		delete catalog;

		return nullptr;
	}
	else if (!catalog)
	{
		Char part_block[sizeof(NFS_ROOT_PARTITION_BLOCK)] = {0};

		drive.fPacket.fPacketContent = part_block;
		drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
		drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

		drive.fInput(&drive.fPacket);

		NFS_ROOT_PARTITION_BLOCK* blk_nefs = (NFS_ROOT_PARTITION_BLOCK*)part_block;
		out_lba							   = blk_nefs->StartCatalog;
	}

	NFS_CATALOG_STRUCT* child_catalog = new NFS_CATALOG_STRUCT();

	child_catalog->ResourceForkSize = 0UL;
	child_catalog->DataForkSize		= 0UL;
	child_catalog->CatalogFlags		= kNeFSStatusUnlocked;
	child_catalog->NextSibling		= out_lba;
	child_catalog->PrevSibling		= out_lba;
	child_catalog->Kind				= kind;
	child_catalog->Flags |= kNeFSFlagCreated;
	child_catalog->CatalogFlags = flags;

	rt_copy_memory((VoidPtr)name, (VoidPtr)child_catalog->Name,
				   rt_string_len(name));

	NFS_CATALOG_STRUCT temporary_catalog{};

	Lba start_free = out_lba;

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	Char buf_part_block[sizeof(NFS_ROOT_PARTITION_BLOCK)] = {0};

	drive.fPacket.fPacketContent = buf_part_block;
	drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
	drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

	drive.fInput(&drive.fPacket);

	NFS_ROOT_PARTITION_BLOCK* part_block = (NFS_ROOT_PARTITION_BLOCK*)buf_part_block;

	if (part_block->FreeCatalog < 1)
	{
		delete child_catalog;
		child_catalog = nullptr;

		return nullptr;
	}

	drive.fPacket.fPacketContent = &temporary_catalog;
	drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
	drive.fPacket.fPacketLba	 = start_free;

	drive.fInput(&drive.fPacket);

	while (start_free >= part_block->StartCatalog)
	{
		// ========================== //
		// Allocate catalog now...
		// ========================== //
		if ((temporary_catalog.Flags & kNeFSFlagCreated) == 0)
		{
			child_catalog->NextSibling =
				start_free + (sizeof(NFS_CATALOG_STRUCT) * kNeFSCatalogPadding);

			drive.fPacket.fPacketContent = &temporary_catalog;
			drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
			drive.fPacket.fPacketLba	 = start_free;

			drive.fOutput(&drive.fPacket);

			child_catalog->DataFork		= part_block->DiskSize - start_free;
			child_catalog->ResourceFork = child_catalog->DataFork;

			drive.fPacket.fPacketContent = child_catalog;
			drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
			drive.fPacket.fPacketLba	 = start_free;

			drive.fOutput(&drive.fPacket);

			// Get NeFS partition's block.

			drive.fPacket.fPacketContent = buf_part_block;
			drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
			drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

			drive.fInput(&drive.fPacket);

			part_block->FreeSectors -= 1;
			part_block->CatalogCount += 1;
			part_block->FreeCatalog -= 1;

			drive.fOutput(&drive.fPacket);

			kcout << "Create new catalog with flags: "
				  << hex_number(child_catalog->Flags) << endl;
			kcout << "Create new catalog with name: " << child_catalog->Name
				  << endl;

			delete catalog;
			catalog = nullptr;

			NFS_CATALOG_STRUCT* found_catalog = new NFS_CATALOG_STRUCT();
			rt_copy_memory(&temporary_catalog, found_catalog, sizeof(NFS_CATALOG_STRUCT));

			return found_catalog;
		}
		else if ((temporary_catalog.Flags & kNeFSFlagCreated) &&
				 StringBuilder::Equals(temporary_catalog.Name, name))
		{
			rt_copy_memory(&temporary_catalog, child_catalog, sizeof(NFS_CATALOG_STRUCT));

			return child_catalog;
		}

		start_free = start_free + (sizeof(NFS_CATALOG_STRUCT) * kNeFSCatalogPadding);

		drive.fPacket.fPacketContent = &temporary_catalog;
		drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
		drive.fPacket.fPacketLba	 = start_free;

		drive.fInput(&drive.fPacket);
	}

	delete catalog;
	return nullptr;
}

/// @brief Make a EPM+NeFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see err_global_get().
bool NeFileSystemParser::Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name)
{
#ifdef ZKA_EPM_SUPPORT
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
		err_global_get() = kErrorDiskIsCorrupted;
		return false;
	}

	Char fs_buf[sizeof(NFS_ROOT_PARTITION_BLOCK)] = {0};

	Lba start = kNeFSRootCatalogStartAddress;

	drive->fPacket.fPacketContent = fs_buf;
	drive->fPacket.fPacketSize	  = sizeof(NFS_ROOT_PARTITION_BLOCK);
	drive->fPacket.fPacketLba	  = start;

	drive->fInput(&drive->fPacket);

	if (flags & kNeFSPartitionTypeBoot)
	{
		// make it bootable when needed.
		Char buf_epm[kNeFSSectorSz] = {0};

		BOOT_BLOCK_STRUCT* epm_boot = (BOOT_BLOCK_STRUCT*)buf_epm;

		// Write a new EPM entry.

		constexpr auto kFsName	  = "NeFS";
		constexpr auto kBlockName = "ZkaOS:";

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kFsName)), epm_boot->Fs, rt_string_len(kFsName));

		epm_boot->FsVersion = kNeFSVersionInteger;
		epm_boot->LbaStart	= start;
		epm_boot->SectorSz	= kNeFSSectorSz;

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), epm_boot->Name, rt_string_len(kBlockName));
		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), epm_boot->Magic, rt_string_len(kEPMMagic));

		Lba outEpmLba = kEPMBootBlockLba;

		Char buf[kNeFSSectorSz];

		Lba	  prevStart = 0;
		SizeT cnt		= 0;

		while (drive->fPacket.fPacketGood)
		{
			drive->fPacket.fPacketContent = buf;
			drive->fPacket.fPacketSize	  = sizeof(EPM_PART_BLOCK);
			drive->fPacket.fPacketLba	  = outEpmLba;

			drive->fInput(&drive->fPacket);

			if (buf[0] == 0)
			{
				epm_boot->LbaStart = prevStart;

				if (epm_boot->LbaStart)
					epm_boot->LbaStart = outEpmLba;

				epm_boot->LbaEnd	= endLba;
				epm_boot->NumBlocks = cnt;

				drive->fPacket.fPacketContent = buf_epm;
				drive->fPacket.fPacketSize	  = sizeof(EPM_PART_BLOCK);
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
			drive->fPacket.fPacketSize	  = sizeof(NFS_ROOT_PARTITION_BLOCK);
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
		drive->fPacket.fPacketSize	  = sizeof(NFS_ROOT_PARTITION_BLOCK);
		drive->fPacket.fPacketLba	  = start;

		drive->fInput(&drive->fPacket);
	}
#endif // ZKA_EPM_SUPPORT

	return false;
}

/// @brief Writes the data fork into a specific catalog.
/// @param catalog the catalog itself
/// @param data the data.
/// @return if the catalog w rote the contents successfully.
bool NeFileSystemParser::WriteCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog, Bool is_rsrc_fork, _Input VoidPtr data, _Input SizeT size_of_data, _Input const Char* forkName)
{
	if (size_of_data < 1)
		return No;

	auto buf = new UInt8[size_of_data];
	rt_set_memory(buf, 0, size_of_data);

	rt_copy_memory(data, buf, size_of_data);

	auto drive = kMountpoint.A();

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
			err_global_get() = kErrorDiskIsCorrupted;

			kcout << "Invalid fork offset.\r";

			return false;
		}

		if (fork_data_input->Flags & kNeFSFlagUnallocated &&
			fork_data_input->Flags & kNeFSFlagDeleted &&
			StringBuilder::Equals(fork_data_input->ForkName, forkName) &&
			StringBuilder::Equals(fork_data_input->CatalogName, catalog->Name) &&
			fork_data_input->DataSize == size_of_data)
		{
			// ===================================================== //
			// Store the blob now.
			// ===================================================== //

			fork_data_input->Flags |= kNeFSFlagCreated;

			drive.fPacket.fPacketContent = buf;
			drive.fPacket.fPacketSize	 = size_of_data;
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
/// @param catalog_name the catalog name.
/// @return the newly found catalog.
_Output NFS_CATALOG_STRUCT* NeFileSystemParser::FindCatalog(_Input const Char* catalog_name,
															Lba&			   out_lba,
															Bool			   search_hidden,
															Bool			   local_search)
{
	if (!catalog_name ||
		*catalog_name == 0)
		return nullptr;

	NFS_ROOT_PARTITION_BLOCK part{0};
	auto&					 drive = kMountpoint.A();

	rt_copy_memory((VoidPtr) "fs/nefs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/nefs-packet"));

	drive.fPacket.fPacketContent = &part;
	drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
	drive.fPacket.fPacketLba	 = kNeFSRootCatalogStartAddress;

	drive.fInput(&drive.fPacket);

	auto	   start_catalog_lba = kNeFSCatalogStartAddress;
	const auto kStartCatalogList = start_catalog_lba;

	if (!StringBuilder::Equals(catalog_name, NeFileSystemHelper::Root()) && local_search)
	{
		Char parent_name[kNeFSNodeNameLen] = {0};

		for (SizeT indexFill = 0; indexFill < rt_string_len(catalog_name); ++indexFill)
		{
			parent_name[indexFill] = catalog_name[indexFill];
		}

		SizeT indexReverseCopy = rt_string_len(parent_name);

		// zero character.
		parent_name[--indexReverseCopy] = 0;

		// mandatory '/' character.
		parent_name[--indexReverseCopy] = 0;

		while (parent_name[indexReverseCopy] != NeFileSystemHelper::Separator())
		{
			parent_name[indexReverseCopy] = 0;
			--indexReverseCopy;
		}

		NFS_CATALOG_STRUCT* parent_catalog = this->FindCatalog(parent_name, out_lba);

		if (parent_catalog &&
			!StringBuilder::Equals(parent_name, NeFileSystemHelper::Root()))
		{
			start_catalog_lba = parent_catalog->NextSibling;

			delete parent_catalog;
			parent_catalog = nullptr;

			local_search = YES;
		}
		else if (parent_catalog)
		{
			start_catalog_lba = parent_catalog->NextSibling;

			local_search = YES;

			delete parent_catalog;
			parent_catalog = nullptr;
		}
		else if (!parent_catalog)
		{
			return nullptr;
		}
	}

	NFS_CATALOG_STRUCT temporary_catalog{};

kNeFSSearchThroughCatalogList:
	while (YES)
	{
		drive.fPacket.fPacketLba	 = start_catalog_lba;
		drive.fPacket.fPacketContent = &temporary_catalog;
		drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);

		drive.fInput(&drive.fPacket);

		kcout << temporary_catalog.Name << endl;

		if (StringBuilder::Equals(catalog_name, temporary_catalog.Name))
		{
			if (temporary_catalog.Status == kNeFSStatusLocked &&
				!search_hidden)
			{
				err_global_get() = kErrorFileLocked;

				goto NeFSContinueSearch;
			}

			/// ignore unallocated catalog, break
			if (!(temporary_catalog.Flags & kNeFSFlagCreated))
			{
				err_global_get() = kErrorFileNotFound;

				goto NeFSContinueSearch;
			}

			kcout << "Found available catalog at: " << hex_number(start_catalog_lba) << endl;
			kcout << "Found available catalog at: " << temporary_catalog.Name << endl;

			NFS_CATALOG_STRUCT* catalog_ptr = new NFS_CATALOG_STRUCT();
			rt_copy_memory(&temporary_catalog, catalog_ptr, sizeof(NFS_CATALOG_STRUCT));

			out_lba = start_catalog_lba;
			return catalog_ptr;
		}

	NeFSContinueSearch:
		start_catalog_lba = temporary_catalog.NextSibling;

		if (start_catalog_lba < part.StartCatalog)
			break;
	}

	if (local_search)
	{
		local_search	  = false;
		start_catalog_lba = part.StartCatalog;

		goto kNeFSSearchThroughCatalogList;
	}

	err_global_get() = kErrorFileNotFound;

	out_lba = 0UL;

	return nullptr;
}

/// @brief Get catalog from filesystem.
/// @param name the catalog's name/
/// @return
_Output NFS_CATALOG_STRUCT* NeFileSystemParser::GetCatalog(_Input const Char* name)
{
	Lba unused = 0;
	return this->FindCatalog(name, unused, YES);
}

/// @brief Closes a catalog, (frees it).
/// @param catalog the catalog to close.
/// @return
Boolean NeFileSystemParser::CloseCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog)
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
Boolean NeFileSystemParser::RemoveCatalog(_Input const Char* catalog_name)
{
	if (!catalog_name ||
		StringBuilder::Equals(catalog_name, NeFileSystemHelper::Root()))
	{
		err_global_get() = kErrorInternal;
		return false;
	}

	Lba	 out_lba = 0;
	auto catalog = this->FindCatalog(catalog_name, out_lba);

	if (out_lba >= kNeFSCatalogStartAddress ||
		catalog->Flags & kNeFSFlagCreated)
	{
		catalog->Flags &= (~kNeFSFlagCreated);
		catalog->Flags |= kNeFSFlagDeleted;

		auto drive = kMountpoint.A();

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
	catalog = nullptr;

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

VoidPtr NeFileSystemParser::ReadCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
										_Input Bool						   is_rsrc_fork,
										_Input SizeT					   dataSz,
										_Input const Char*				   forkName)
{
	if (!catalog)
	{
		err_global_get() = kErrorFileNotFound;
		return nullptr;
	}

	Lba	 dataForkLba  = (!is_rsrc_fork) ? catalog->DataFork : catalog->ResourceFork;
	Size dataForkSize = (!is_rsrc_fork) ? catalog->DataForkSize : catalog->ResourceForkSize;

	kcout << "catalog " << catalog->Name
		  << ", fork: " << hex_number(dataForkLba) << endl;

	NFS_FORK_STRUCT* fs_buf = new NFS_FORK_STRUCT();
	auto			 drive	= kMountpoint.A();

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

bool NeFileSystemParser::Seek(_Input _Output NFS_CATALOG_STRUCT* catalog, SizeT off)
{
	if (!catalog)
	{
		err_global_get() = kErrorFileNotFound;
		return false;
	}

	err_global_get() = kErrorUnimplemented;
	return false;
}

/***********************************************************************************/
/// @brief Tell where we are inside the data fork.
/// @param catalog
/// @return The position on the file.
/***********************************************************************************/

SizeT NeFileSystemParser::Tell(_Input _Output NFS_CATALOG_STRUCT* catalog)
{
	if (!catalog)
	{
		err_global_get() = kErrorFileNotFound;
		return 0;
	}

	err_global_get() = kErrorUnimplemented;
	return 0;
}

namespace Kernel::NeFS
{
	/***********************************************************************************/
	/// @brief Construct NeFS drives.
	/***********************************************************************************/
	Boolean fs_init_nefs(Void) noexcept
	{
		kcout << "Creating A:\r";

		kMountpoint.A() = io_construct_main_drive();

		kcout << "Creating A: [ OK ]\r";

		return true;
	}
} // namespace Kernel::NeFS

#endif // ifdef __FSKIT_INCLUDES_NEFS__
