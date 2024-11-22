/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

	FILE: NeFS.h
	PURPOSE: NeFS (New FileSystem) support.

	Revision History:

	?/?/?: Added file (amlel)
	12/02/24: Add UUID macro for EPM and GPT partition schemes.
	3/16/24: Add mandatory sector size, kNeFSSectorSz is set to 2048 by
default.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <HintKit/CompilerHint.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Defines.h>

/**
	@brief New File System specification.
	@author EL Mahrouss Logic
*/

#define kNeFSInvalidFork	(-1)
#define kNeFSInvalidCatalog (-1)
#define kNeFSNodeNameLen	(256)

#define kNeFSMinimumDiskSize (gib_cast(4))

#define kNeFSSectorSz	(512)
#define kNeFSForkDataSz (mib_cast(16))

#define kNeFSIdentLen (8)
#define kNeFSIdent	  "  NeFS"
#define kNeFSPadLen	  (392)

#define kNeFSMetaFilePrefix '$'

#define kNeFSVersionInteger (0x0129)
#define kNeFSVerionString	"1.2.9"

/// @brief Standard fork types.
#define kNeFSDataFork	  "main_data"
#define kNeFSResourceFork "main_rsrc"

#define kNeFSForkSize (sizeof(NFS_FORK_STRUCT))

#define kNeFSPartitionTypeStandard (7)
#define kNeFSPartitionTypePage	   (8)
#define kNeFSPartitionTypeBoot	   (9)

#define kNeFSCatalogKindFile  (1)
#define kNeFSCatalogKindDir	  (2)
#define kNeFSCatalogKindAlias (3)

//! Shared between network and/or partitions. Export forks as .zip when copying.
#define kNeFSCatalogKindShared (4)

#define kNeFSCatalogKindResource   (5)
#define kNeFSCatalogKindExecutable (6)

#define kNeFSCatalogKindPage (8)

#define kNeFSCatalogKindDevice (9)
#define kNeFSCatalogKindLock   (10)

#define kNeFSCatalogKindRLE		 (11)
#define kNeFSCatalogKindMetaFile (12)

#define kNeFSCatalogKindTTF	 (13)
#define kNeFSCatalogKindRIFF (14)

#define kNeFSSeparator	  '/'
#define kNeFSSeparatorAlt '/'

#define kNeFSUpDir	 ".."
#define kNeFSRoot	 "/"
#define kNeFSRootAlt "/"

#define kNeFSLF	 '\r'
#define kNeFSEOF (-1)

#define kNeFSBitWidth (sizeof(Kernel::Char))
#define kNeFSLbaType  (Kernel::Lba)

/// @note Start after the partition map header. (Virtual addressing)
#define kNeFSRootCatalogStartAddress (1024)
#define kNeFSCatalogStartAddress	 ((2048) + sizeof(NFS_ROOT_PARTITION_BLOCK))

#define kResourceTypeDialog (10)
#define kResourceTypeString (11)
#define kResourceTypeMenu	(12)
#define kResourceTypeSound	(13)
#define kResourceTypeFont	(14)

#define kConfigLen (64)
#define kPartLen   (32)

#define kNeFSFlagDeleted	 (70)
#define kNeFSFlagUnallocated (0)
#define kNeFSFlagCreated	 (71)

#define kNeFSMimeNameLen (200)
#define kNeFSForkNameLen (200)

struct NFS_CATALOG_STRUCT;
struct NFS_FORK_STRUCT;
struct NFS_ROOT_PARTITION_BLOCK;

enum
{
	kNeFSHardDrive		   = 0xC0, // Hard Drive
	kNeFSSolidStateDrive   = 0xC1, // Solid State Drive
	kNeFSOpticalDrive	   = 0x0C, // Blu-Ray/DVD
	kNeFSMassStorageDevice = 0xCC, // USB
	kNeFSScsiDrive		   = 0xC4, // SCSI Hard Drive
	kNeFSFlashDrive		   = 0xC6,
	kNeFSUnknown		   = 0xFF, // Unknown device.
	kNeFSDriveCount		   = 7,
};

/// @brief Catalog type.
struct PACKED NFS_CATALOG_STRUCT final
{
	Kernel::Char Name[kNeFSNodeNameLen];
	Kernel::Char Mime[kNeFSMimeNameLen];

	/// Catalog status flag.
	Kernel::UInt16 Flags;
	/// Custom catalog flags.
	Kernel::UInt16 FilkMMFlags;
	/// Catalog kind.
	Kernel::Int32 Kind;

	/// Size of the data fork.
	Kernel::Lba DataForkSize;

	/// Size of all resource forks.
	Kernel::Lba ResourceForkSize;

	Kernel::Lba DataFork;
	Kernel::Lba ResourceFork;

	Kernel::Lba NextSibling;
	Kernel::Lba PrevSibling;
};

/// @brief Fork type, contains a data page.
/// @note The way we store is way different than how other filesystems do, specific chunk of code are
/// written into either the data fork or resource fork, the resource fork is reserved for file metadata.
/// whereas the data fork is reserved for file data.
struct PACKED NFS_FORK_STRUCT final
{
	Kernel::Char ForkName[kNeFSForkNameLen];
	Kernel::Char CatalogName[kNeFSNodeNameLen];

	Kernel::Int32 Flags;
	Kernel::Int32 Kind;

	Kernel::Int64 ResourceId;
	Kernel::Int32 ResourceKind;
	Kernel::Int32 ResourckMMFlags;

	Kernel::Lba	  DataOffset; // 8 Where to look for this data?
	Kernel::SizeT DataSize;	  /// Data size according using sector count.

	Kernel::Lba NextSibling;
	Kernel::Lba PreviousSibling;
};

/// @brief Partition block type
struct PACKED NFS_ROOT_PARTITION_BLOCK final
{
	Kernel::Char Ident[kNeFSIdentLen];
	Kernel::Char PartitionName[kPartLen];

	Kernel::Int32 Flags;
	Kernel::Int32 Kind;

	Kernel::Lba	  StartCatalog;
	Kernel::SizeT CatalogCount;

	Kernel::SizeT DiskSize;

	Kernel::SizeT FreeCatalog;
	Kernel::SizeT FreeSectors;

	Kernel::SizeT SectorCount;
	Kernel::SizeT SectorSize;

	Kernel::UInt64 Version;

	Kernel::Lba EpmBlock;

	Kernel::Char Pad[kNeFSPadLen];
};

namespace Kernel
{
	enum
	{
		kNeFSSubDriveA,
		kNeFSSubDriveB,
		kNeFSSubDriveC,
		kNeFSSubDriveD,
		kNeFSSubDriveInvalid,
		kNeFSSubDriveCount,
	};

	/// \brief Resource fork kind.
	enum
	{
		kNeFSRsrcForkKind = 0,
		kNeFSDataForkKind = 1
	};

	///
	/// \name NeFSParser
	/// \brief NeFS parser class. (catalog creation, remove removal, root,
	/// forks...) Designed like the DOM, detects the filesystem automatically.
	///
	class NeFSParser final
	{
	public:
		explicit NeFSParser() = default;
		~NeFSParser()		  = default;

	public:
		ZKA_COPY_DEFAULT(NeFSParser);

	public:
		/// @brief Creates a new fork inside the New filesystem partition.
		/// @param catalog it's catalog
		/// @param theFork the fork itself.
		/// @return the fork
		_Output NFS_FORK_STRUCT* CreateFork(_Input NFS_CATALOG_STRUCT* catalog,
											_Input NFS_FORK_STRUCT& theFork);

		/// @brief Find fork inside New filesystem.
		/// @param catalog the catalog.
		/// @param name the fork name.
		/// @return the fork.
		_Output NFS_FORK_STRUCT* FindFork(_Input NFS_CATALOG_STRUCT* catalog,
										  _Input const Char* name,
										  Boolean			 dataOrRsrc);

		_Output Void RemoveFork(_Input NFS_FORK_STRUCT* fork);

		_Output Void CloseFork(_Input NFS_FORK_STRUCT* fork);

		_Output NFS_CATALOG_STRUCT* FindCatalog(_Input const Char* catalogName, Lba& outLba);

		_Output NFS_CATALOG_STRUCT* GetCatalog(_Input const Char* name);

		_Output NFS_CATALOG_STRUCT* CreateCatalog(_Input const Char* name,
												  _Input const Int32& flags,
												  _Input const Int32& kind);

		_Output NFS_CATALOG_STRUCT* CreateCatalog(_Input const Char* name);

		Bool WriteCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
						  _Input Bool						 isRsrcFork,
						  _Input VoidPtr					 data,
						  _Input SizeT						 sizeOfData,
						  _Input const Char* forkName);

		VoidPtr ReadCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
							_Input Bool						   isRsrcFork,
							_Input SizeT					   dataSz,
							_Input const Char* forkName);

		bool Seek(_Input _Output NFS_CATALOG_STRUCT* catalog, SizeT off);

		SizeT Tell(_Input _Output NFS_CATALOG_STRUCT* catalog);

		bool RemoveCatalog(_Input const Char* catalog);

		bool CloseCatalog(_InOut NFS_CATALOG_STRUCT* catalog);

		/// @brief Make a EPM+NeFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see ErrLocal().
		bool Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name);

	public:
		Int32 fDriveIndex{kNeFSSubDriveA};
	};

	///
	/// \name NeFileSystemHelper
	/// \brief Filesystem helper and utils.
	///

	class NeFileSystemHelper final
	{
	public:
		STATIC const Char* Root();
		STATIC const Char* UpDir();
		STATIC const Char  Separator();
		STATIC const Char  MetaFile();
	};

	namespace Detail
	{
		Boolean fs_init_newfs(Void) noexcept;
	} // namespace Detail
} // namespace Kernel

/// @brief Write to newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Kernel::Int32 fs_newfs_write(Kernel::MountpointInterface* Mnt,
							 Kernel::DriveTrait&		  DrvTrait,
							 Kernel::Int32				  DrvIndex);

/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Kernel::Int32 fs_newfs_read(Kernel::MountpointInterface* Mnt,
							Kernel::DriveTrait&			 DrvTrait,
							Kernel::Int32				 DrvIndex);
