/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifdef __FSKIT_NEWFS__

#include <FSKit/NewFS.hxx>
#include <NewKit/Utils.hpp>

using namespace NewOS;

STATIC Lba ke_find_free_fork(SizeT sz);
STATIC Lba ke_find_free_catalog(SizeT sz);
STATIC Lba ke_find_free_data(SizeT sz);

STATIC MountpointInterface sMountpointInterface;

/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param theFork the fork itself.
/// @return the fork
_Output NewFork* NewFSParser::CreateFork(_Input NewCatalog* catalog,
                                         _Input NewFork& theFork) {
  if (catalog && theFork.Name[0] != 0 && theFork.DataSize > 0) {
    Lba whereFork = 0;

    theFork.DataOffset = ke_find_free_fork(theFork.DataSize);
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

  return true;
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
  return false;
}

/// @brief 
/// @param catalog 
/// @return 
Boolean NewFSParser::RemoveCatalog(_Input _Output NewCatalog* catalog) {
  return false;
}

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

/// @brief 
/// @param sz 
/// @return 
STATIC Lba ke_find_free_fork(SizeT sz) { return 0; }

/// @brief 
/// @param sz 
/// @return 
STATIC Lba ke_find_free_catalog(SizeT sz) { return 0; }

/// @brief 
/// @param sz 
/// @return 
STATIC Lba ke_find_free_data(SizeT sz) { return 0; }

namespace NewOS::Detail {
Boolean fs_init_newfs(Void) noexcept { return false; }
}  // namespace NewOS::Detail

#endif  // ifdef __FSKIT_NEWFS__