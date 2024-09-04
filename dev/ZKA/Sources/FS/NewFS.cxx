/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifdef __FSKIT_USE_NEWFS__

#include <Modules/AHCI/AHCI.hxx>
#include <Modules/ATA/ATA.hxx>
#include <Modules/Flash/Flash.hxx>
#include <FSKit/NewFS.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/Crc32.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Utils.hxx>
#include <FirmwareKit/EPM.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/User.hxx>

using namespace Kernel;

#ifdef __ED__
/***********************************************************************************/
/**
	Define those external symbols, to make the editor shutup
*/
/***********************************************************************************/

/***********************************************************************************/
/// @brief get sector count.
/***********************************************************************************/
Kernel::SizeT drv_std_get_sector_count();

/***********************************************************************************/
/// @brief get device size.
/***********************************************************************************/
Kernel::SizeT drv_std_get_drv_size();

#endif

///! BUGS: 0

/***********************************************************************************/
/// This file implements the New File System.
///	New File System implements a B-Tree based algortihm.
///		\\
///	\\Path1\\		\\ath2\\
/// \\readme.rtf		\\ListContents.pef \\readme.lnk <-- symlink.
///								\\Path1\\readme.rtf
/***********************************************************************************/

STATIC MountpointInterface sMountpointInterface;

/***********************************************************************************/
/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param theFork the fork itself.
/// @return the fork
/***********************************************************************************/
_Output NFS_FORK_STRUCT* NewFSParser::CreateFork(_Input NFS_CATALOG_STRUCT* catalog,
												 _Input NFS_FORK_STRUCT& theFork)
{
	if (catalog && theFork.ForkName[0] != 0)
	{
		Lba lba = (theFork.Kind == kNewFSDataForkKind) ? catalog->DataFork
													   : catalog->ResourceFork;

		kcout << "newoskrnl.exe: fork lba: " << hex_number(lba) << endl;

		if (lba <= kNewFSCatalogStartAddress)
			return nullptr;

		auto drv = sMountpointInterface.A();

		/// special treatment.
		rt_copy_memory((VoidPtr) "fs/newfs-packet", drv.fPacket.fPacketMime,
					   rt_string_len("fs/newfs-packet"));

		NFS_FORK_STRUCT curFork{0};
		NFS_FORK_STRUCT prevFork{0};
		Lba				lbaOfPreviousFork = lba;

		/// do not check for anything. Loop until we get what we want, that is a free fork zone.
		while (true)
		{
			if (lba <= kNewFSCatalogStartAddress)
				break;

			drv.fPacket.fLba		   = lba;
			drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
			drv.fPacket.fPacketContent = &curFork;

			drv.fInput(&drv.fPacket);

			if (curFork.NextSibling > kBadAddress)
			{
				kcout << "newoskrnl.exe: bad fork: " << hex_number(curFork.NextSibling) << endl;
				break;
			}

			kcout << "newoskrnl.exe: next fork: " << hex_number(curFork.NextSibling) << endl;

			if (curFork.Flags == kNewFSFlagCreated)
			{
				kcout << "newoskrnl.exe: fork already exists.\r";

				/// sanity check.
				if (StringBuilder::Equals(curFork.ForkName, theFork.ForkName) &&
					StringBuilder::Equals(curFork.CatalogName, catalog->Name))
					return nullptr;

				kcout << "newoskrnl.exe: next fork: " << hex_number(curFork.NextSibling) << endl;

				lbaOfPreviousFork = lba;
				lba				  = curFork.NextSibling;

				prevFork = curFork;
			}
			else
			{
				/// This is a check that we have, in order to link the previous fork
				/// entry.
				if (lba >= kNewFSCatalogStartAddress)
				{
					drv.fPacket.fLba		   = lbaOfPreviousFork;
					drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
					drv.fPacket.fPacketContent = &prevFork;

					prevFork.NextSibling = lba;

					/// write to disk.
					drv.fOutput(&drv.fPacket);
				}

				break;
			}
		}

		constexpr auto cForkPadding =
			4; /// this value gives us space for the data offset.

		theFork.Flags			= kNewFSFlagCreated;
		theFork.DataOffset		= lba - sizeof(NFS_FORK_STRUCT) * cForkPadding;
		theFork.PreviousSibling = lbaOfPreviousFork;
		theFork.NextSibling		= theFork.DataOffset - theFork.DataSize;

		drv.fPacket.fLba		   = lba;
		drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
		drv.fPacket.fPacketContent = &theFork;

		drv.fOutput(&drv.fPacket);

		/// log what we have now.
		kcout << "newoskrnl.exe: Wrote fork data at: " << hex_number(theFork.DataOffset)
			  << endl;

		kcout << "newoskrnl.exe: Wrote fork at: " << hex_number(lba) << endl;

		return &theFork;
	}

	return nullptr;
}

/***********************************************************************************/
/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
/***********************************************************************************/
_Output NFS_FORK_STRUCT* NewFSParser::FindFork(_Input NFS_CATALOG_STRUCT* catalog,
											   _Input const Char* name,
											   Boolean			  isDataFork)
{
	auto			 drv	 = sMountpointInterface.A();
	NFS_FORK_STRUCT* theFork = nullptr;

	Lba lba = isDataFork ? catalog->DataFork : catalog->ResourceFork;

	while (lba != 0)
	{
		drv.fPacket.fLba		   = lba;
		drv.fPacket.fPacketSize	   = sizeof(NFS_FORK_STRUCT);
		drv.fPacket.fPacketContent = (VoidPtr)theFork;

		rt_copy_memory((VoidPtr) "fs/newfs-packet", drv.fPacket.fPacketMime, 16);

		if (auto res =
				fs_newfs_read(&sMountpointInterface, drv, this->fDriveIndex);
			res)
		{
			switch (res)
			{
			case 1:
				ErrLocal() = kErrorDiskReadOnly;
				break;
			case 2:
				ErrLocal() = kErrorDiskIsFull;
				break;
				ErrLocal() = kErrorNoSuchDisk;
				break;

			default:
				break;
			}
			return nullptr;
		}

		if (StringBuilder::Equals(theFork->ForkName, name))
		{
			break;
		}

		lba = theFork->NextSibling;
	}

	return theFork;
}

/***********************************************************************************/
/// @brief Simpler factory to create a catalog (assumes you want to create a
/// file.)
/// @param name
/// @return catalog pointer.
/***********************************************************************************/
_Output NFS_CATALOG_STRUCT* NewFSParser::CreateCatalog(_Input const Char* name)
{
	return this->CreateCatalog(name, 0, kNewFSCatalogKindFile);
}

/***********************************************************************************/
/// @brief Creates a new catalog into the disk.
/// @param name the catalog name.
/// @param flags the flags of the catalog.
/// @param kind the catalog kind.
/// @return catalog pointer.
/***********************************************************************************/
_Output NFS_CATALOG_STRUCT* NewFSParser::CreateCatalog(_Input const Char* name,
													   _Input const Int32& flags,
													   _Input const Int32& kind)
{
	kcout << "newoskrnl.exe: CreateCatalog(...)\r";

	Lba out_lba = 0UL;

	kcout << "newoskrnl.exe: Checking for extension...\r";

	/// a directory should have a slash in the end.
	if (kind == kNewFSCatalogKindDir &&
		name[rt_string_len(name) - 1] != NewFilesystemHelper::Separator())
		return nullptr;

	/// a file shouldn't have a slash in the end.
	if (kind != kNewFSCatalogKindDir &&
		name[rt_string_len(name) - 1] == NewFilesystemHelper::Separator())
		return nullptr;

	NFS_CATALOG_STRUCT* catalog_copy = this->FindCatalog(name, out_lba);

	if (catalog_copy)
	{
		kcout << "newoskrnl.exe: Catalog already exists: " << name << ".\r";
		ErrLocal() = kErrorFileExists;

		return catalog_copy;
	}

	Char parentName[kNewFSNodeNameLen] = {0};

	for (SizeT indexName = 0UL; indexName < rt_string_len(name); ++indexName)
	{
		parentName[indexName] = name[indexName];
	}

	if (*parentName == 0)
	{
		kcout << "newoskrnl.exe: Parent name is NUL.\r";
		ErrLocal() = kErrorFileNotFound;
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

	while (parentName[indexReverseCopy] != NewFilesystemHelper::Separator())
	{
		parentName[indexReverseCopy] = 0;
		--indexReverseCopy;
	}

	NFS_CATALOG_STRUCT* catalog = this->FindCatalog(parentName, out_lba);

	auto drive = sMountpointInterface.A();

	if (catalog && catalog->Kind == kNewFSCatalogKindFile)
	{
		kcout << "newoskrnl.exe: Parent name is file.\r";
		delete catalog;
		return nullptr;
	}
	else if (!catalog)
	{
		Char sectorBufPartBlock[kNewFSSectorSz] = {0};

		drive.fPacket.fPacketContent = sectorBufPartBlock;
		drive.fPacket.fPacketSize	 = kNewFSSectorSz;
		drive.fPacket.fLba			 = kNewFSRootCatalogStartAddress;

		drive.fInput(&drive.fPacket);

		constexpr auto cNewFSCatalogPadding = 4;

		NFS_ROOT_PARTITION_BLOCK* partBlock = (NFS_ROOT_PARTITION_BLOCK*)sectorBufPartBlock;
		out_lba								= partBlock->StartCatalog;
	}

	constexpr SizeT cDefaultForkSize = kNewFSForkSize;

	NFS_CATALOG_STRUCT* catalogChild = new NFS_CATALOG_STRUCT();

	Int32 flagsList = flags;

	catalogChild->ResourceForkSize = cDefaultForkSize;
	catalogChild->DataForkSize	   = cDefaultForkSize;

	catalogChild->NextSibling = out_lba;
	catalogChild->PrevSibling = out_lba;
	catalogChild->Kind		  = kind;
	catalogChild->Flags		  = kNewFSFlagCreated | flagsList;

	rt_copy_memory((VoidPtr)name, (VoidPtr)catalogChild->Name,
				   rt_string_len(name));

	UInt16 catalogBuf[kNewFSSectorSz] = {0};

	Lba start_free = out_lba;

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	drive.fPacket.fPacketContent = catalogBuf;
	drive.fPacket.fPacketSize	 = kNewFSSectorSz;
	drive.fPacket.fLba			 = start_free;

	drive.fInput(&drive.fPacket);

	NFS_CATALOG_STRUCT* nextSibling = (NFS_CATALOG_STRUCT*)catalogBuf;

	start_free = nextSibling->NextSibling;

	catalogChild->PrevSibling = out_lba;

	drive.fPacket.fLba = start_free;
	drive.fInput(&drive.fPacket);

	while (drive.fPacket.fPacketGood)
	{
		nextSibling = reinterpret_cast<NFS_CATALOG_STRUCT*>(catalogBuf);

		if (start_free <= kNewFSRootCatalogStartAddress)
		{
			delete catalogChild;
			delete catalog;

			return nullptr;
		}

		// ========================== //
		// Allocate catalog now...
		// ========================== //
		if ((nextSibling->Flags & kNewFSFlagCreated) == 0)
		{
			Char sectorBufPartBlock[kNewFSSectorSz] = {0};

			drive.fPacket.fPacketContent = sectorBufPartBlock;
			drive.fPacket.fPacketSize	 = kNewFSSectorSz;
			drive.fPacket.fLba			 = kNewFSRootCatalogStartAddress;

			drive.fInput(&drive.fPacket);

			constexpr auto cNewFSCatalogPadding = 4;

			NFS_ROOT_PARTITION_BLOCK* partBlock = (NFS_ROOT_PARTITION_BLOCK*)sectorBufPartBlock;

			if (partBlock->FreeCatalog < 1)
			{
				delete catalogChild;
				return nullptr;
			}

			catalogChild->DataFork	   = partBlock->DiskSize - start_free;
			catalogChild->ResourceFork = catalogChild->DataFork;

			// Write the new catalog next sibling, if we don't know this parent. //
			// This is necessary, so that we don't have to get another lba to allocate. //
			if (!StringBuilder::Equals(parentName, nextSibling->Name))
			{
				catalogChild->NextSibling =
					start_free + (sizeof(NFS_CATALOG_STRUCT) * cNewFSCatalogPadding);
			}

			drive.fPacket.fPacketContent = catalogChild;
			drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);
			drive.fPacket.fLba			 = start_free;

			drive.fOutput(&drive.fPacket);

			// Get NewFS partition's block.

			drive.fPacket.fPacketContent = sectorBufPartBlock;
			drive.fPacket.fPacketSize	 = kNewFSSectorSz;
			drive.fPacket.fLba			 = kNewFSRootCatalogStartAddress;

			drive.fInput(&drive.fPacket);

			partBlock->FreeSectors -= 1;
			partBlock->CatalogCount += 1;
			partBlock->FreeCatalog -= 1;

			drive.fOutput(&drive.fPacket);

			kcout << "newoskrnl.exe: Create new catalog, status: "
				  << hex_number(catalogChild->Flags) << endl;
			kcout << "newoskrnl.exe: Create new catalog, name: " << catalogChild->Name
				  << endl;

			delete catalog;
			return catalogChild;
		}
		else if ((nextSibling->Flags & kNewFSFlagCreated) &&
				 StringBuilder::Equals(nextSibling->Name, name))
		{
			return nextSibling;
		}

		constexpr auto cNewFSCatalogPadding = 4;

		//// @note that's how we find the next catalog in the partition block.
		start_free = start_free + (sizeof(NFS_CATALOG_STRUCT) * cNewFSCatalogPadding);

		drive.fPacket.fPacketContent = catalogBuf;
		drive.fPacket.fPacketSize	 = kNewFSSectorSz;
		drive.fPacket.fLba			 = start_free;

		drive.fInput(&drive.fPacket);
	}

	delete catalog;
	return nullptr;
}

/// @brief Make a EPM+NewFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see ErrLocal().
bool NewFSParser::Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name)
{
	if (*part_name == 0 ||
		endLba == 0)
		return false;

	// verify disk.
	drive->fVerify(&drive->fPacket);

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	// if disk isn't good, then error out.
	if (false == drive->fPacket.fPacketGood)
	{
		ErrLocal() = kErrorDiskIsCorrupted;
		return false;
	}

	Char fs_buf[kNewFSSectorSz] = {0};

	Lba start = kNewFSRootCatalogStartAddress;

	drive->fPacket.fPacketContent = fs_buf;
	drive->fPacket.fPacketSize	  = kNewFSSectorSz;
	drive->fPacket.fLba			  = start;

	drive->fInput(&drive->fPacket);

	if (flags & kNewFSPartitionTypeBoot)
	{
		// make it bootable when needed.
		Char bufEpmHdr[kNewFSSectorSz] = {0};

		BOOT_BLOCK_STRUCT* epmBoot = (BOOT_BLOCK_STRUCT*)bufEpmHdr;

		// EPM header.

		constexpr auto cFsName	  = "NewFS";
		constexpr auto cBlockName = "ZKA:";

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(cFsName)), epmBoot->Fs, rt_string_len(cFsName));

		epmBoot->FsVersion = kNewFSVersionInteger;
		epmBoot->LbaStart  = start;
		epmBoot->SectorSz  = kNewFSSectorSz;

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(cBlockName)), epmBoot->Name, rt_string_len(cBlockName));
		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), epmBoot->Magic, rt_string_len(kEPMMagic));

		Lba outEpmLba = kEpmBase;

		Char buf[kNewFSSectorSz];

		Lba	  prevStart = 0;
		SizeT cnt		= 0;

		while (drive->fPacket.fPacketGood)
		{
			drive->fPacket.fPacketContent = buf;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba			  = outEpmLba;

			drive->fInput(&drive->fPacket);

			if (buf[0] == 0)
			{
				epmBoot->LbaStart = prevStart;

				if (epmBoot->LbaStart)
					epmBoot->LbaStart = outEpmLba;

				epmBoot->LbaEnd	   = endLba;
				epmBoot->NumBlocks = cnt;

				drive->fPacket.fPacketContent = bufEpmHdr;
				drive->fPacket.fPacketSize	  = kNewFSSectorSz;
				drive->fPacket.fLba			  = outEpmLba;

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
		NFS_ROOT_PARTITION_BLOCK* partBlock = (NFS_ROOT_PARTITION_BLOCK*)fs_buf;

		// check for an empty partition here.
		if (partBlock->PartitionName[0] == 0 &&
			rt_string_cmp(partBlock->Ident, kNewFSIdent, kNewFSIdentLen))
		{
			// partition is free and valid.

			partBlock->Version = kNewFSVersionInteger;

			const auto cUntitledHD = part_name;

			rt_copy_memory((VoidPtr)kNewFSIdent, (VoidPtr)partBlock->Ident,
						   kNewFSIdentLen);

			rt_copy_memory((VoidPtr)cUntitledHD, (VoidPtr)partBlock->PartitionName,
						   rt_string_len(cUntitledHD));

			SizeT catalogCount = 0UL;

			SizeT sectorCount = drv_std_get_sector_count();
			SizeT diskSize	  = drv_std_get_drv_size();

			partBlock->Kind			= kNewFSPartitionTypeStandard;
			partBlock->StartCatalog = kNewFSCatalogStartAddress;
			partBlock->Flags		= kNewFSPartitionTypeStandard;
			partBlock->CatalogCount = sectorCount / sizeof(NFS_CATALOG_STRUCT);
			partBlock->SectorCount	= sectorCount;
			partBlock->DiskSize		= diskSize;
			partBlock->FreeCatalog	= sectorCount / sizeof(NFS_CATALOG_STRUCT);

			drive->fPacket.fPacketContent = fs_buf;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba			  = kNewFSRootCatalogStartAddress;

			drive->fOutput(&drive->fPacket);

			kcout << "newoskrnl.exe: drive kind: " << drive->fDriveKind() << endl;

			kcout << "newoskrnl.exe: partition name: " << partBlock->PartitionName << endl;
			kcout << "newoskrnl.exe: start: " << hex_number(partBlock->StartCatalog) << endl;
			kcout << "newoskrnl.exe: number of catalogs: " << hex_number(partBlock->CatalogCount) << endl;
			kcout << "newoskrnl.exe: free catalog: " << hex_number(partBlock->FreeCatalog) << endl;
			kcout << "newoskrnl.exe: free sectors: " << hex_number(partBlock->FreeSectors) << endl;
			kcout << "newoskrnl.exe: sector size: " << hex_number(partBlock->SectorSize) << endl;

			// write the root catalog.
			this->CreateCatalog(kNewFSRoot, 0, kNewFSCatalogKindDir);

			return true;
		}

		kcout << "newoskrnl.exe: partition block already exists.\r";

		start += partBlock->DiskSize;

		drive->fPacket.fPacketContent = fs_buf;
		drive->fPacket.fPacketSize	  = kNewFSSectorSz;
		drive->fPacket.fLba			  = start;

		drive->fInput(&drive->fPacket);
	}

	return false;
}

/// @brief Writes the data fork into a specific catalog.
/// @param catalog the catalog itself
/// @param data the data.
/// @return if the catalog w rote the contents successfully.
bool NewFSParser::WriteCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog, Bool isRsrcFork, _Input VoidPtr data, _Input SizeT sizeOfData, _Input const Char* forkName)
{
	auto drive = sMountpointInterface.A();

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	auto startFork = (!isRsrcFork) ? catalog->DataFork
								   : catalog->ResourceFork;

	NFS_FORK_STRUCT* forkDataIn = new NFS_FORK_STRUCT();
	NFS_FORK_STRUCT	 prevFork{};

	// sanity check of the fork position as the condition to run the loop.
	while (startFork >= kNewFSCatalogStartAddress)
	{
		drive.fPacket.fPacketContent = forkDataIn;
		drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
		drive.fPacket.fLba			 = startFork;

		drive.fInput(&drive.fPacket);

		// check the fork, if it's position is valid.
		if (forkDataIn->DataOffset <= kNewFSCatalogStartAddress)
		{
			ErrLocal() = kErrorDiskIsCorrupted;

			kcout << "newoskrnl.exe: Invalid fork offset.\r";

			return false;
		}

		if (forkDataIn->Flags != kNewFSFlagUnallocated &&
			forkDataIn->Flags != kNewFSFlagDeleted &&
			StringBuilder::Equals(forkDataIn->ForkName, forkName) &&
			StringBuilder::Equals(forkDataIn->CatalogName, catalog->Name))
		{
			// ===================================================== //
			// Store size of blob now.
			// ===================================================== //

			if (forkDataIn->DataSize < sizeOfData &&
				forkDataIn->DataSize < 1)
			{
				startFork = forkDataIn->NextSibling;
				continue;
			}

			forkDataIn->Flags	   = kNewFSFlagCreated;
			forkDataIn->DataOffset = startFork + sizeof(NFS_FORK_STRUCT);
			forkDataIn->DataSize   = sizeOfData;

			drive.fPacket.fPacketContent = data;
			drive.fPacket.fPacketSize	 = sizeOfData;
			drive.fPacket.fLba			 = startFork + sizeof(NFS_FORK_STRUCT);

			kcout << "newoskrnl.exe: data offset: " << hex_number(forkDataIn->DataOffset) << endl;

			drive.fOutput(&drive.fPacket);

			drive.fPacket.fPacketContent = &forkDataIn;
			drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
			drive.fPacket.fLba			 = startFork;

			drive.fOutput(&drive.fPacket);

			kcout << "newoskrnl.exe: wrote fork at offset: " << hex_number(forkDataIn->DataOffset) << endl;

			delete catalog;

			return true;
		}

		// stumble upon a fork, store it.

		prevFork = *forkDataIn;

		startFork = forkDataIn->NextSibling;
	}

	return false;
}

/// @brief
/// @param catalogName the catalog name.
/// @return the newly found catalog.
_Output NFS_CATALOG_STRUCT* NewFSParser::FindCatalog(_Input const Char* catalogName,
													 Lba&				out_lba)
{
	kcout << "newoskrnl.exe: start finding catalog...\r";

	NFS_ROOT_PARTITION_BLOCK fs_buf{0};
	auto					 drive = sMountpointInterface.A();

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	drive.fPacket.fPacketContent = &fs_buf;
	drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);
	drive.fPacket.fLba			 = kNewFSRootCatalogStartAddress;

	drive.fInput(&drive.fPacket);

	NFS_ROOT_PARTITION_BLOCK* part = (NFS_ROOT_PARTITION_BLOCK*)&fs_buf;

	auto	   startCatalogList	 = part->StartCatalog;
	const auto cCtartCatalogList = startCatalogList;

	auto localSearchFirst = false;

	NFS_CATALOG_STRUCT catalogBuf{0};

	drive.fPacket.fLba			 = startCatalogList;
	drive.fPacket.fPacketContent = &catalogBuf;
	drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);

	drive.fInput(&drive.fPacket);

	if (!StringBuilder::Equals(catalogName, NewFilesystemHelper::Root()))
	{
		Char parentName[kNewFSNodeNameLen] = {0};

		for (SizeT indexFill = 0; indexFill < rt_string_len(catalogName); ++indexFill)
		{
			parentName[indexFill] = catalogName[indexFill];
		}

		SizeT indexReverseCopy = rt_string_len(parentName);

		// zero character.
		parentName[--indexReverseCopy] = 0;

		// mandatory '/' character.
		parentName[--indexReverseCopy] = 0;

		while (parentName[indexReverseCopy] != NewFilesystemHelper::Separator())
		{
			parentName[indexReverseCopy] = 0;
			--indexReverseCopy;
		}

		NFS_CATALOG_STRUCT* parentCatalog = this->FindCatalog(parentName, out_lba);

		if (parentCatalog &&
			!StringBuilder::Equals(parentName, NewFilesystemHelper::Root()))
		{
			startCatalogList = parentCatalog->NextSibling;
			delete parentCatalog;

			localSearchFirst = true;
		}
		else if (parentCatalog)
		{
			delete parentCatalog;
		}
	}

	kcout << "newoskrnl.exe: fetching catalog...\r";

NewFSSearchThroughCatalogList:
	while (drive.fPacket.fPacketGood)
	{
		drive.fPacket.fLba			 = startCatalogList;
		drive.fPacket.fPacketContent = &catalogBuf;
		drive.fPacket.fPacketSize	 = sizeof(NFS_CATALOG_STRUCT);

		drive.fInput(&drive.fPacket);

		NFS_CATALOG_STRUCT* catalog = (NFS_CATALOG_STRUCT*)&catalogBuf;

		if (StringBuilder::Equals(catalogName, catalog->Name))
		{
			/// ignore unallocated catalog, break
			if (!(catalog->Flags & kNewFSFlagCreated))
			{
				goto NewFSContinueSearch;
			}

			NFS_CATALOG_STRUCT* catalogPtr = new NFS_CATALOG_STRUCT();
			rt_copy_memory(catalog, catalogPtr, sizeof(NFS_CATALOG_STRUCT));

			kcout << "newoskrnl.exe: found catalog at: " << hex_number(startCatalogList) << endl;
			kcout << "newoskrnl.exe: found catalog at: " << catalog->Name << endl;

			out_lba = startCatalogList;
			return catalogPtr;
		}

	NewFSContinueSearch:
		startCatalogList = catalog->NextSibling;

		if (startCatalogList <= kNewFSRootCatalogStartAddress)
			break;
	}

	if (localSearchFirst)
	{
		localSearchFirst = false;
		startCatalogList = cCtartCatalogList;

		goto NewFSSearchThroughCatalogList;
	}

	out_lba = 0UL;
	return nullptr;
}

/// @brief Get catalog from filesystem.
/// @param name the catalog's name/
/// @return
_Output NFS_CATALOG_STRUCT* NewFSParser::GetCatalog(_Input const Char* name)
{
	Lba unused = 0;
	return this->FindCatalog(name, unused);
}

/// @brief Closes a catalog, (frees it).
/// @param catalog the catalog to close.
/// @return
Boolean NewFSParser::CloseCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog)
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
Boolean NewFSParser::RemoveCatalog(_Input const Char* catalogName)
{
	if (!catalogName ||
		StringBuilder::Equals(catalogName, NewFilesystemHelper::Root()))
	{
		ErrLocal() = kErrorInternal;
		return false;
	}

	Lba	 out_lba = 0;
	auto catalog = this->FindCatalog(catalogName, out_lba);

	if (out_lba >= kNewFSCatalogStartAddress ||
		catalog->Flags == kNewFSFlagCreated)
	{
		catalog->Flags = kNewFSFlagDeleted;

		auto drive = sMountpointInterface.A();

		rt_copy_memory((VoidPtr) "fs/newfs-packet", drive.fPacket.fPacketMime,
					   rt_string_len("fs/newfs-packet"));

		drive.fPacket.fLba = out_lba; // the catalog position.
		drive.fPacket.fPacketSize =
			sizeof(NFS_CATALOG_STRUCT);			// size of catalog. roughly the sector size.
		drive.fPacket.fPacketContent = catalog; // the catalog itself.

		drive.fOutput(&drive.fPacket); // send packet.

		Char partitionBlockBuf[sizeof(NFS_ROOT_PARTITION_BLOCK)] = {0};

		drive.fPacket.fLba			 = kNewFSRootCatalogStartAddress;
		drive.fPacket.fPacketContent = partitionBlockBuf;
		drive.fPacket.fPacketSize	 = sizeof(NFS_ROOT_PARTITION_BLOCK);

		drive.fInput(&drive.fPacket);

		NFS_ROOT_PARTITION_BLOCK* partBlock =
			reinterpret_cast<NFS_ROOT_PARTITION_BLOCK*>(partitionBlockBuf);

		--partBlock->CatalogCount;
		++partBlock->FreeSectors;

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

VoidPtr NewFSParser::ReadCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
								 _Input Bool						isRsrcFork,
								 _Input SizeT						dataSz,
								 _Input const Char* forkName)
{
	if (!catalog)
	{
		ErrLocal() = kErrorFileNotFound;
		return nullptr;
	}

	constexpr auto cNewFSCatalogPadding = 4;

	Lba	 dataForkLba  = (!isRsrcFork) ? catalog->DataFork : catalog->ResourceFork;
	Size dataForkSize = (!isRsrcFork) ? catalog->DataForkSize : catalog->ResourceForkSize;

	kcout << "newoskrnl.exe: catalog " << catalog->Name
		  << ", fork: " << hex_number(dataForkLba) << endl;

	NFS_FORK_STRUCT* fs_buf = new NFS_FORK_STRUCT();
	auto			 drive	= sMountpointInterface.A();

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive.fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	NFS_FORK_STRUCT* fs_fork_data = nullptr;

	while (dataForkLba > kNewFSCatalogStartAddress)
	{
		drive.fPacket.fLba			 = dataForkLba;
		drive.fPacket.fPacketSize	 = sizeof(NFS_FORK_STRUCT);
		drive.fPacket.fPacketContent = fs_buf;

		drive.fInput(&drive.fPacket);

		fs_fork_data = fs_buf;

		kcout << "newoskrnl.exe: ForkName: " << fs_fork_data->ForkName << endl;
		kcout << "newoskrnl.exe: CatalogName: " << fs_fork_data->CatalogName << endl;

		if (StringBuilder::Equals(forkName, fs_fork_data->ForkName) &&
			StringBuilder::Equals(catalog->Name, fs_fork_data->CatalogName))
			break;

		dataForkLba = fs_fork_data->NextSibling;
	}

	if (dataForkLba < kNewFSCatalogStartAddress)
	{
		delete[] fs_buf;
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

bool NewFSParser::Seek(_Input _Output NFS_CATALOG_STRUCT* catalog, SizeT off)
{
	if (!catalog)
	{
		ErrLocal() = kErrorFileNotFound;
		return false;
	}

	ErrLocal() = kErrorUnimplemented;
	return false;
}

/***********************************************************************************/
/// @brief Tell where we are inside the data fork.
/// @param catalog
/// @return The position on the file.
/***********************************************************************************/

SizeT NewFSParser::Tell(_Input _Output NFS_CATALOG_STRUCT* catalog)
{
	if (!catalog)
	{
		ErrLocal() = kErrorFileNotFound;
		return 0;
	}

	ErrLocal() = kErrorUnimplemented;
	return 0;
}

namespace Kernel::Detail
{
	/***********************************************************************************/
	/// @brief Construct NewFS drives.
	/***********************************************************************************/
	Boolean fs_init_newfs(Void) noexcept
	{
		kcout << "newoskrnl.exe: Creating drives...\r";

		sMountpointInterface.A() = io_construct_main_drive();
		sMountpointInterface.B() = io_construct_drive();
		sMountpointInterface.C() = io_construct_drive();
		sMountpointInterface.D() = io_construct_drive();

		kcout << "newoskrnl.exe: Testing A:\r";

		sMountpointInterface.A().fVerify(&sMountpointInterface.A().fPacket);

		kcout << "newoskrnl.exe: Testing A: [ OK ]\r";

		return true;
	}
} // namespace Kernel::Detail

#endif // ifdef __FSKIT_USE_NEWFS__
