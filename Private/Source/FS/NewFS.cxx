/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifdef __FSKIT_NEWFS__

#include <FSKit/NewFS.hxx>
#include <KernelKit/HError.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/Utils.hpp>

using namespace NewOS;

/// forward decl.

STATIC Lba ke_find_free_fork(SizeT sz, Int32 drv, NewCatalog* catalog);
STATIC Lba ke_find_free_catalog(SizeT kind, Int32 drv);

STATIC MountpointInterface sMountpointInterface;

/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param theFork the fork itself.
/// @return the fork
_Output NewFork* NewFSParser::CreateFork(_Input NewCatalog* catalog,
                                         _Input NewFork& theFork) {
  if (catalog && theFork.Name[0] != 0 && theFork.DataSize > 0) {
    Lba whereFork = 0;

    theFork.DataOffset =
        ke_find_free_fork(theFork.DataSize, this->fDriveIndex, catalog);
    theFork.Flags |= kNewFSFlagCreated;

    if (catalog->FirstFork == 0) {
      catalog->FirstFork = whereFork;
    } else {
      if (catalog->LastFork == 0) {
        theFork.PreviousSibling = catalog->FirstFork;
      }
    }

    if (catalog->LastFork == 0) {
      catalog->LastFork = whereFork;
    } else {
      theFork.PreviousSibling = catalog->LastFork;
    }

    if (!sMountpointInterface.GetAddressOf(this->fDriveIndex) ||
        !*sMountpointInterface.GetAddressOf(this->fDriveIndex))
      return nullptr;

    auto drv = *sMountpointInterface.GetAddressOf(this->fDriveIndex);

    drv->fPacket.fLba = whereFork;
    drv->fPacket.fPacketSize = theFork.DataSize;
    drv->fPacket.fPacketContent = (VoidPtr)&theFork;

    rt_copy_memory((VoidPtr) "fs/newfs-packet", drv->fPacket.fPacketMime, 16);

    if (auto res =
            fs_newfs_write(&sMountpointInterface, *drv, this->fDriveIndex);
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

    /// Also update catalog.
    this->WriteCatalog(catalog, nullptr);

    return &theFork;
  }

  return nullptr;
}

/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
_Output NewFork* NewFSParser::FindFork(_Input NewCatalog* catalog,
                                       _Input const Char* name) {
  auto drv = *sMountpointInterface.GetAddressOf(this->fDriveIndex);
  NewFork* theFork = nullptr;
  Lba lba = catalog->FirstFork;

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

      rt_copy_memory((VoidPtr)kNewFSIdent, (VoidPtr)partBlock->Ident,
                     kNewFSIdentLen);
      rt_copy_memory((VoidPtr) "New OS\0", (VoidPtr)partBlock->PartitionName,
                     rt_string_len("New OS\0"));

      SizeT catalogCount = 0;
      SizeT sectorCount = 0;
      SizeT diskSize = 0;

      partBlock->Kind = kNewFSPartitionTypeStandard;
      partBlock->StartCatalog = sizeof(NewPartitionBlock) + kNewFSAddressAsLba;
      partBlock->CatalogCount = catalogCount;
      partBlock->SectorCount = sectorCount;
      partBlock->DiskSize = diskSize;
      partBlock->FreeCatalog = partBlock->StartCatalog;

      drive->fPacket.fPacketContent = sectorBuf;
      drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
      drive->fPacket.fLba = kNewFSAddressAsLba;

      drive->fOutput(&drive->fPacket);

      return true;
    }

    kcout << "New OS: Partition already exists.\r\n";

    /// return success as well, do not ignore that partition.
    return true;
  }

  return false;
}

/// @brief
/// @param catalog
/// @param data
/// @return
bool NewFSParser::WriteCatalog(_Input _Output NewCatalog* catalog,
                               voidPtr data) {
  return false;
}

/// @brief
/// @param catalogName
/// @return
_Output NewCatalog* NewFSParser::FindCatalog(_Input const char* catalogName) {
  return nullptr;
}

/// @brief
/// @param name
/// @return
_Output NewCatalog* NewFSParser::GetCatalog(_Input const char* name) {
  return nullptr;
}

/// @brief
/// @param catalog
/// @return
Boolean NewFSParser::CloseCatalog(_Input _Output NewCatalog* catalog) {
  return true;
}

/// @brief Mark catalog as removed.
/// @param catalog The catalog structure.
/// @return
Boolean NewFSParser::RemoveCatalog(_Input _Output NewCatalog* catalog) {
  catalog->Flags |= kNewFSFlagDeleted;
  this->WriteCatalog(catalog, nullptr);

  return false;
}

/// ***************************************************************** ///
/// Reading,Seek,Tell are unimplemented on catalogs, refer to forks I/O instead.
/// ***************************************************************** ///

/// @brief
/// @param catalog
/// @param dataSz
/// @return
VoidPtr NewFSParser::ReadCatalog(_Input _Output NewCatalog* catalog,
                                 SizeT dataSz) {
  return nullptr;
}

/// @brief
/// @param catalog
/// @param off
/// @return
bool NewFSParser::Seek(_Input _Output NewCatalog* catalog, SizeT off) {
  return false;
}

/// @brief
/// @param catalog
/// @return
SizeT NewFSParser::Tell(_Input _Output NewCatalog* catalog) { return 0; }

/// @brief Find a free fork inside the filesystem.
/// @param sz the size of the fork to set.
/// @return the valid lba.
STATIC Lba ke_find_free_fork(SizeT sz, Int32 drv, NewCatalog* catalog) {
  auto drive = *sMountpointInterface.GetAddressOf(drv);

  if (drive) {
    /// prepare packet.
    bool done = false;
    bool error = false;

    Lba lba = catalog->LastFork;

    while (!done) {
      Char sectorBuf[kNewFSMinimumSectorSz] = {0};

      drive->fPacket.fPacketContent = sectorBuf;
      drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
      drive->fPacket.fLba = lba;

      drive->fInput(&drive->fPacket);

      if (!drive->fPacket.fPacketGood) {
        ///! not a lot of choices, disk has become unreliable.
        if (ke_calculate_crc32(sectorBuf, kNewFSMinimumSectorSz) !=
            drive->fPacket.fPacketCRC32) {
          DbgLastError() = kErrorDiskIsCorrupted;
        }

        error = true;
        break;
      }

      NewFork* fork = (NewFork*)sectorBuf;

      if (fork->DataSize == 0 && fork->Name[0] == 0 &&
          (fork->Flags == kNewFSFlagDeleted ||
           fork->Flags == kNewFSFlagUnallocated)) {
        fork->DataSize = sz;
        fork->Flags |= kNewFSFlagCreated;

        drive->fOutput(&drive->fPacket);

        /// here it's either a read-only filesystem or something bad happened.'
        if (!drive->fPacket.fPacketGood) {
          DbgLastError() = kErrorDiskReadOnly;

          return 0;
        }

        return lba;
      }

      lba += sizeof(NewFork);
    }

    if (error) {
      DbgLastError() = kErrorDisk;
      return 0;
    }
  }

  return 0;
}

/// @brief find a free catalog.
/// @param kind the catalog kind.
/// @return the valid lba.
STATIC Lba ke_find_free_catalog(SizeT kind, Int32 drv) {
  auto drive = *sMountpointInterface.GetAddressOf(drv);

  if (drive) {
    Char sectorBuf[kNewFSMinimumSectorSz] = {0};

    /// prepare packet.

    drive->fPacket.fPacketContent = sectorBuf;
    drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
    drive->fPacket.fLba = kNewFSAddressAsLba;

    drive->fInput(&drive->fPacket);

    NewPartitionBlock* partBlock = (NewPartitionBlock*)sectorBuf;

    /// check for a valid partition.
    if (partBlock->PartitionName[0] != 0 &&
        rt_string_cmp(partBlock->Ident, kNewFSIdent, kNewFSIdentLen) == 0) {
      auto startLba = partBlock->FreeCatalog;

      if (startLba == 0) {
        DbgLastError() = kErrorDiskIsFull;
        return 1;
      } else {
        while (startLba != 0) {
          drive->fPacket.fPacketContent = sectorBuf;
          drive->fPacket.fPacketSize = kNewFSMinimumSectorSz;
          drive->fPacket.fLba = startLba;

          drive->fInput(&drive->fPacket);

          NewCatalog* catalog = (NewCatalog*)sectorBuf;

          if (catalog->Flags == kNewFSFlagUnallocated ||
              catalog->Flags == kNewFSFlagDeleted) {
            catalog->Flags |= kNewFSFlagCreated;
            catalog->Kind |= kind;

            return startLba;
          }

          startLba = catalog->NextSibling;
        }

        return 0;
      }
    }
  }

  return 0;
}

namespace NewOS::Detail {
Boolean fs_init_newfs(Void) noexcept { return true; }
}  // namespace NewOS::Detail

#endif  // ifdef __FSKIT_NEWFS__
