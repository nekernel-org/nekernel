/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifdef __FSKIT_NEWFS__

#include <Builtins/AHCI/AHCI.hxx>
#include <Builtins/ATA/ATA.hxx>
#include <FSKit/NewFS.hxx>
#include <KernelKit/HError.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>

using namespace NewOS;

STATIC MountpointInterface sMountpointInterface;

/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param theFork the fork itself.
/// @return the fork
_Output NewFork* NewFSParser::CreateFork(_Input NewCatalog* catalog,
                                         _Input NewFork& theFork) {
  if (catalog && theFork.Name[0] != 0 && theFork.DataSize > 0) {
    theFork.Flags = kNewFSFlagCreated;
    Lba lba = (theFork.Kind == kNewFSDataForkKind) ? catalog->DataFork
                                                   : catalog->ResourceFork;

    if (lba <= kNewFSCatalogStartAddress) return nullptr;

    theFork.DataOffset = lba + sizeof(NewCatalog) + sizeof(NewFork);

    if (!sMountpointInterface.GetAddressOf(this->fDriveIndex)) return nullptr;

    auto drv = sMountpointInterface.GetAddressOf(this->fDriveIndex);

    Char cpyFork[sizeof(NewFork)] = {0};

    drv->fPacket.fLba = lba + sizeof(NewCatalog);
    drv->fPacket.fPacketSize = sizeof(NewFork);
    drv->fPacket.fPacketContent = (VoidPtr)cpyFork;

    drv->fInput(&drv->fPacket);

    NewFork* cpyForkStruct = (NewFork*)cpyFork;

    if (cpyForkStruct->Flags == kNewFSFlagCreated) return nullptr;

    /// special treatment.
    rt_copy_memory((VoidPtr) "fs/newfs-packet", drv->fPacket.fPacketMime,
                   rt_string_len("fs/newfs-packet"));

    drv->fPacket.fLba = lba;
    drv->fPacket.fPacketSize = sizeof(NewFork);
    drv->fPacket.fPacketContent = (VoidPtr)&theFork;

    drv->fOutput(&drv->fPacket);

    return &theFork;
  }

  return nullptr;
}

/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
_Output NewFork* NewFSParser::FindFork(_Input NewCatalog* catalog,
                                       _Input const Char* name,
                                       Boolean isDataFork) {
  auto drv = sMountpointInterface.GetAddressOf(this->fDriveIndex);
  NewFork* theFork = nullptr;
  Lba lba = isDataFork ? catalog->DataFork : catalog->ResourceFork;

  while (lba != 0) {
    drv->fPacket.fLba = lba;
    drv->fPacket.fPacketSize = sizeof(NewFork);
    drv->fPacket.fPacketContent = (VoidPtr)theFork;

    rt_copy_memory((VoidPtr) "fs/newfs-packet", drv->fPacket.fPacketMime, 16);

    if (auto res =
            fs_newfs_read(&sMountpointInterface, *drv, this->fDriveIndex);
        res) {
      switch (res) {
        case 1:
          DbgLastError() = kErrorDiskReadOnly;
          break;
        case 2:
          DbgLastError() = kErrorDiskIsFull;
          break;
          DbgLastError() = kErrorNoSuchDisk;
          break;

        default:
          break;
      }
      return nullptr;
    }

    if (StringBuilder::Equals(theFork->Name, name)) {
      break;
    }

    lba = theFork->NextSibling;
  }

  return theFork;
}

/// @brief Simpler factory to create a catalog (assumes you want to create a
/// file.)
/// @param name
/// @return
_Output NewCatalog* NewFSParser::CreateCatalog(_Input const char* name) {
  return this->CreateCatalog(name, 0, kNewFSCatalogKindFile);
}

/// @brief
/// @param name
/// @param flags
/// @param kind
/// @return
_Output NewCatalog* NewFSParser::CreateCatalog(_Input const char* name,
                                               _Input const Int32& flags,
                                               _Input const Int32& kind) {
  if (!sMountpointInterface.GetAddressOf(this->fDriveIndex)) return nullptr;

  Lba outLba = 0UL;

  if (kind == kNewFSCatalogKindDir &&
      name[rt_string_len(name) - 1] != NewFilesystemHelper::Separator())
    return nullptr;

  /// as well as this, we don't want that.
  if (kind != kNewFSCatalogKindDir &&
      name[rt_string_len(name) - 1] == NewFilesystemHelper::Separator())
    return nullptr;

  NewCatalog* checkForCpy = this->FindCatalog(name, outLba);

  if (checkForCpy) {
    return checkForCpy;
  }

  char copyName[kNewFSNodeNameLen] = {0};

  for (SizeT indexName = 0UL; indexName < rt_string_len(name); ++indexName) {
    copyName[indexName] = name[indexName];
  }

  if (*copyName == 0) {
    DbgLastError() = kErrorFileNotFound;
    return nullptr;
  }

  for (SizeT indexFill = 0; indexFill < rt_string_len(name); ++indexFill) {
    copyName[indexFill] = name[indexFill];
  }

  SizeT indexReverseCopy = rt_string_len(copyName);

  // zero character.
  copyName[--indexReverseCopy] = 0;

  // mandatory / character.
  copyName[--indexReverseCopy] = 0;

  while (copyName[indexReverseCopy] != '/') {
    copyName[indexReverseCopy] = 0;
    --indexReverseCopy;
  }

  NewCatalog* catalog = this->FindCatalog(copyName, outLba);

  if (!catalog) {
    return nullptr;
  }

  if (catalog->Kind == kNewFSCatalogKindFile) {
    delete catalog;
    return nullptr;
  }

  constexpr SizeT cDefaultForkSize = 8192;

  NewCatalog* catalogChild = new NewCatalog();

  catalogChild->ResourceForkOverallSize = cDefaultForkSize;
  catalogChild->DataForkSize = cDefaultForkSize;

  catalogChild->NextSibling = 0UL;
  catalogChild->PrevSibling = outLba;
  catalogChild->Kind = kind;
  catalogChild->Flags = kNewFSFlagCreated;

  rt_copy_memory((VoidPtr)name, (VoidPtr)catalogChild->Name,
                 rt_string_len(name));

  UInt16 sectorBuf[kNewFSMinimumSectorSz] = {0};

  auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

  Lba startFree = catalogChild->PrevSibling + catalog->NextSibling;

  kcout << "Next-Catalog: " << hex_number(startFree) << endl;

  drive->fPacket.fPacketContent = sectorBuf;
  drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
  drive->fPacket.fLba = startFree;

  drive->fInput(&drive->fPacket);

  while (drive->fPacket.fPacketGood) {
    NewCatalog* nextSibling = (NewCatalog*)sectorBuf;

    if (startFree <= kNewFSAddressAsLba) {
      delete catalogChild;
      delete catalog;

      return nullptr;
    }

    /// allocation or reallocation or catalog...
    if (nextSibling->Name[0] == 0 && nextSibling->Flags != kNewFSFlagCreated) {
      catalogChild->DataFork = startFree + sizeof(NewCatalog);
      catalogChild->ResourceFork =
          startFree + sizeof(NewCatalog) + catalogChild->DataForkSize;

      if (catalogChild->Kind == kNewFSCatalogKindFile) {
        rt_copy_memory((VoidPtr) "x-kind/file", (VoidPtr)catalogChild->Mime,
                       rt_string_len("x-kind/file"));
      } else {
        rt_copy_memory((VoidPtr) "x-kind/dir", (VoidPtr)catalogChild->Mime,
                       rt_string_len("x-kind/dir"));
      }

      catalogChild->NextSibling = sizeof(NewCatalog) +
                                  catalogChild->DataForkSize +
                                  catalogChild->ResourceForkOverallSize;

      drive->fPacket.fPacketContent = catalogChild;
      drive->fPacket.fPacketSize = sizeof(NewCatalog);
      drive->fPacket.fLba = startFree;

      drive->fOutput(&drive->fPacket);

      kcout << "New OS: Create new catalog successfully!\r";

      Char sectBuf[sizeof(NewCatalog)] = {0};

      drive->fPacket.fPacketContent = sectBuf;
      drive->fPacket.fPacketSize = sizeof(NewCatalog);
      drive->fPacket.fLba = catalogChild->PrevSibling;

      drive->fInput(&drive->fPacket);

      NewCatalog* prevCatalog = (NewCatalog*)sectBuf;
      prevCatalog->NextSibling = startFree;

      drive->fOutput(&drive->fPacket);

      kcout << "Edit-Catalog: " << prevCatalog->Name << endl;

      Char sectorBufPartBlock[kNewFSMinimumSectorSz] = {0};

      drive->fPacket.fPacketContent = sectorBufPartBlock;
      drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
      drive->fPacket.fLba = kNewFSAddressAsLba;

      drive->fInput(&drive->fPacket);

      NewPartitionBlock* partBlock = (NewPartitionBlock*)sectorBufPartBlock;

      partBlock->SectorCount -= 1;
      partBlock->CatalogCount += 1;
      partBlock->FreeCatalog -= 1;

      drive->fOutput(&drive->fPacket);

      delete catalog;
      return catalogChild;
    }

    //// @note that's how we find the next catalog in the partition block.
    startFree += sizeof(NewFork) + sizeof(NewCatalog) +
                 nextSibling->DataForkSize +
                 nextSibling->ResourceForkOverallSize;

    drive->fPacket.fPacketContent = sectorBuf;
    drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
    drive->fPacket.fLba = startFree;

    drive->fInput(&drive->fPacket);
  }

  delete catalog;
  return nullptr;
}

/// @brief Make a EPM+NewFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see DbgLastError().
bool NewFSParser::Format(_Input _Output DriveTrait* drive) {
  /// verify disk.
  drive->fVerify(&drive->fPacket);

  /// if disk isn't good, then error out.
  if (false == drive->fPacket.fPacketGood) {
    DbgLastError() = kErrorDiskIsCorrupted;
    return false;
  }

  Char sectorBuf[kNewFSMinimumSectorSz] = {0};

  drive->fPacket.fPacketContent = sectorBuf;
  drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
  drive->fPacket.fLba = kNewFSAddressAsLba;

  drive->fInput(&drive->fPacket);

  /// disk isnt faulty and data has been fetched.
  if (drive->fPacket.fPacketGood) {
    NewPartitionBlock* partBlock = (NewPartitionBlock*)sectorBuf;

    /// check for an empty partition here.
    if (partBlock->PartitionName[0] == 0 &&
        rt_string_cmp(partBlock->Ident, kNewFSIdent, kNewFSIdentLen)) {
      /// partition is free and valid.

      partBlock->Version = kNewFSVersionInteger;

      const auto cUntitledHD = "New OS HD\0";

      rt_copy_memory((VoidPtr)kNewFSIdent, (VoidPtr)partBlock->Ident,
                     kNewFSIdentLen);

      rt_copy_memory((VoidPtr)cUntitledHD, (VoidPtr)partBlock->PartitionName,
                     rt_string_len(cUntitledHD));

      SizeT catalogCount = 0;
      SizeT sectorCount = drv_std_get_sector_count();
      SizeT diskSize = drv_std_get_drv_size();

      partBlock->Kind = kNewFSPartitionTypeStandard;
      partBlock->StartCatalog = kNewFSCatalogStartAddress;
      partBlock->Flags |= kNewFSPartitionTypeStandard;
      partBlock->CatalogCount = sectorCount / sizeof(NewCatalog);
      partBlock->SectorCount = sectorCount;
      partBlock->DiskSize = diskSize;
      partBlock->FreeCatalog = partBlock->StartCatalog;

      drive->fPacket.fPacketContent = sectorBuf;
      drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
      drive->fPacket.fLba = kNewFSAddressAsLba;

      drive->fOutput(&drive->fPacket);

      kcout << "Drive-Kind: " << drive->fDriveKind() << endl;

      kcout << "Partition-Name: " << partBlock->PartitionName << endl;
      kcout << "Start-Catalog: " << number(partBlock->StartCatalog) << endl;
      kcout << "Catalog-Count: " << number(partBlock->CatalogCount) << endl;
      kcout << "Free-Catalog: " << number(partBlock->FreeCatalog) << endl;
      kcout << "Free-Sectors: " << number(partBlock->FreeSectors) << endl;
      kcout << "Sector-Size: " << number(partBlock->SectorSize) << endl;

      return true;
    }

    kcout << "New OS: PartitionBlock already exists.\r";

    /// return success as well, do not ignore that partition.
    return true;
  }

  return false;
}

/// @brief Writes the data fork into a specific catalog.
/// @param catalog the catalog itself
/// @param data the data.
/// @return if the catalog w rote the contents successfully.
bool NewFSParser::WriteCatalog(_Input _Output NewCatalog* catalog, voidPtr data,
                               SizeT sizeOfData) {
  if (sizeOfData > catalog->DataForkSize) return false;
  if (!sMountpointInterface.GetAddressOf(this->fDriveIndex)) return false;

  NewFork* forkData = new NewFork();

  auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

  auto startFork = catalog->DataFork;

  /// sanity check of the fork position as the condition to run the loop.
  if (startFork >= kNewFSCatalogStartAddress) {
    drive->fPacket.fPacketContent = forkData;
    drive->fPacket.fPacketSize = sizeof(NewFork);
    drive->fPacket.fLba = startFork;

    drive->fInput(&drive->fPacket);

    /// check if fork even exists.
    if (forkData->Flags != kNewFSFlagCreated) {
      DbgLastError() = kErrorDiskIsCorrupted;

      delete forkData;
      return false;
    }

    /// sanity check the fork.
    if (forkData->DataOffset <= kNewFSCatalogStartAddress) {
      DbgLastError() = kErrorDiskIsCorrupted;

      delete forkData;
      return false;
    }

    drive->fPacket.fPacketContent = data;
    drive->fPacket.fPacketSize = sizeOfData;
    drive->fPacket.fLba = forkData->DataOffset;

    drive->fOutput(&drive->fPacket);

    return true;
  }

  return false;
}

/// @brief
/// @param catalogName
/// @return
_Output NewCatalog* NewFSParser::FindCatalog(_Input const char* catalogName,
                                             Lba& outLba) {
  if (!sMountpointInterface.GetAddressOf(this->fDriveIndex)) return nullptr;

  Char* sectorBuf = new Char[sizeof(NewPartitionBlock)];
  auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

  drive->fPacket.fPacketContent = sectorBuf;
  drive->fPacket.fPacketSize = sizeof(NewPartitionBlock);
  drive->fPacket.fLba = kNewFSAddressAsLba;

  drive->fInput(&drive->fPacket);

  NewPartitionBlock* part = (NewPartitionBlock*)sectorBuf;

  auto start = part->StartCatalog;

  drive->fPacket.fLba = start;
  drive->fPacket.fPacketContent = sectorBuf;
  drive->fPacket.fPacketSize = sizeof(NewCatalog);

  drive->fInput(&drive->fPacket);

  while (drive->fPacket.fPacketGood) {
    NewCatalog* catalog = (NewCatalog*)sectorBuf;

    kcout << "Catalog-Name: " << catalog->Name << endl;
    kcout << "Catalog-Flags: " << hex_number(catalog->Flags) << endl;

    if (StringBuilder::Equals(catalogName, catalog->Name)) {
      /// ignore unallocated catalog, break
      if (catalog->Flags != kNewFSFlagCreated) {

        goto _NewFSContinueSearch;
      }

      NewCatalog* catalogPtr = new NewCatalog();
      rt_copy_memory(catalog, catalogPtr, sizeof(NewCatalog));

      outLba = start;
      delete[] sectorBuf;
      return catalogPtr;
    }

  _NewFSContinueSearch:
    start = catalog->NextSibling;

    kcout << "Catalog-Next: " << number(start) << endl;

    if (start <= kNewFSAddressAsLba) break;

    drive->fPacket.fLba = start;
    drive->fPacket.fPacketContent = sectorBuf;
    drive->fPacket.fPacketSize = sizeof(NewCatalog);

    drive->fInput(&drive->fPacket);
  }

  outLba = 0UL;
  delete[] sectorBuf;

  return nullptr;
}

/// @brief
/// @param name
/// @return
_Output NewCatalog* NewFSParser::GetCatalog(_Input const char* name) {
  Lba unused = 0;
  return this->FindCatalog(name, unused);
}

/// @brief
/// @param catalog
/// @return
Boolean NewFSParser::CloseCatalog(_Input _Output NewCatalog* catalog) {
  if (this->WriteCatalog(catalog, nullptr, 0)) {
    delete catalog;
    catalog = nullptr;

    return true;
  }

  return false;
}

/// @brief Mark catalog as removed.
/// @param catalog The catalog structure.
/// @return
Boolean NewFSParser::RemoveCatalog(_Input const Char* catalogName) {
  if (!catalogName) {
    DbgLastError() = kErrorInternal;
    return false;
  }

  Lba outLba = 0;
  auto catalog = this->FindCatalog(catalogName, outLba);

  if (outLba >= kNewFSCatalogStartAddress ||
      catalog->Flags == kNewFSFlagCreated) {
    catalog->Flags = kNewFSFlagDeleted;

    auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

    drive->fPacket.fLba = outLba;  // the catalog position.
    drive->fPacket.fPacketSize =
        sizeof(NewCatalog);  // size of catalog. roughly the sector size.
    drive->fPacket.fPacketContent = catalog;  // the catalog itself.

    drive->fOutput(&drive->fPacket);  // send packet.

    return true;
  }

  delete catalog;
  return false;
}

/// ***************************************************************** ///
/// Reading,Seek,Tell are unimplemented on catalogs, refer to forks I/O instead.
/// ***************************************************************** ///

/// @brief Read the catalog data fork.
/// @param catalog
/// @param dataSz
/// @return
VoidPtr NewFSParser::ReadCatalog(_Input _Output const char* catalogName,
                                 SizeT dataSz) {
  if (!catalogName) {
    DbgLastError() = kErrorFileNotFound;
    return nullptr;
  }

  if (!sMountpointInterface.GetAddressOf(this->fDriveIndex)) return nullptr;

  NewCatalog* catalog = this->GetCatalog(catalogName);

  if (!catalog) return nullptr;

  Char* sectorBuf = new Char[sizeof(NewFork)];
  auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

  drive->fPacket.fLba = catalog->DataFork;
  drive->fPacket.fPacketSize = sizeof(NewFork);
  drive->fPacket.fPacketContent = sectorBuf;

  drive->fInput(&drive->fPacket);

  NewFork* forkData = (NewFork*)sectorBuf;

  kcout << forkData->Name << " " << hex_number(forkData->DataOffset) << endl;

  if (dataSz > forkData->DataSize) {
    delete[] sectorBuf;

    return nullptr;
  }

  Char* forkBuf = new Char[dataSz];

  drive->fPacket.fLba = forkData->DataOffset;
  drive->fPacket.fPacketSize = dataSz;
  drive->fPacket.fPacketContent = forkBuf;

  drive->fInput(&drive->fPacket);

  delete[] sectorBuf;

  return forkBuf;
}

/// @brief Seek in the data fork.
/// @param catalog the catalog offset.
/// @param off where to seek.
/// @return
bool NewFSParser::Seek(_Input _Output NewCatalog* catalog, SizeT off) {
  if (!catalog) {
    DbgLastError() = kErrorFileNotFound;
    return false;
  }

  return true;
}

/// @brief Tell where we are inside the data fork.
/// @param catalog
/// @return
SizeT NewFSParser::Tell(_Input _Output NewCatalog* catalog) {
  if (!catalog) {
    DbgLastError() = kErrorFileNotFound;
    return 0;
  }

  return 0;
}

namespace NewOS::Detail {
Boolean fs_init_newfs(Void) noexcept {
  sMountpointInterface.A() = construct_main_drive();
  sMountpointInterface.B() = construct_drive();
  sMountpointInterface.C() = construct_drive();
  sMountpointInterface.D() = construct_drive();

  sMountpointInterface.A().fVerify(&sMountpointInterface.A().fPacket);

  return true;
}
}  // namespace NewOS::Detail

#endif  // ifdef __FSKIT_NEWFS__
