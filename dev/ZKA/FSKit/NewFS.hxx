/* -------------------------------------------

	Copyright ZKA Technologies.

	File: NewFS.hxx
	Purpose:

	Revision History:

	?/?/?: Added file (amlel)
	12/02/24: Add UUID macro for EPM and GPT partition schemes.
	3/16/24: Add mandatory sector size, kNewFSSectorSz is set to 2048 by
default.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/Defines.hxx>

/**
	@brief New File System specification.
	@author Amlal EL Mahrouss
*/

#define kNewFSInvalidFork	 (-1)
#define kNewFSInvalidCatalog (-1)
#define kNewFSNodeNameLen	 (256)

#define kNewFSSectorSz (512)
#define kNewFSForkSz   (8192)

#define kNewFSIdentLen (8)
#define kNewFSIdent	   " NewFS"
#define kNewFSPadLen   (400)

#define kNewFSMetaFilePrefix '$'

#define kNewFSVersionInteger (0x0128)
#define kNewFSVerionString	 "1.28"

/// @brief Standard fork types.
#define kNewFSDataFork	   "main_data"
#define kNewFSResourceFork "main_rsrc"

#define kNewFSCatalogKindFile  (1)
#define kNewFSCatalogKindDir   (2)
#define kNewFSCatalogKindAlias (3)

#define kNewFSForkSize (512)

//! shared between network or
//! other filesystems. Export forks as .zip when copying.
#define kNewFSCatalogKindShared (4)

#define kNewFSCatalogKindResource	(5)
#define kNewFSCatalogKindExecutable (6)

#define kNewFSCatalogKindPage (8)

#define kNewFSPartitionTypeStandard (7)
#define kNewFSPartitionTypePage		(8)
#define kNewFSPartitionTypeBoot		(9)

#define kNewFSCatalogKindDevice (9)
#define kNewFSCatalogKindLock	(10)

#define kNewFSCatalogKindRLE (11)

#define kNewFSCatalogKindMetaFile (12)

#define kNewFSSeparator	   '\\'
#define kNewFSSeparatorAlt '/'

#define kNewFSUpDir	  ".."
#define kNewFSRoot	  "\\"
#define kNewFSRootAlt "/"

#define kNewFSLF  '\r'
#define kNewFSEOF (-1)

#define kNewFSBitWidth (sizeof(Kernel::Char))
#define kNewFSLbaType  (Kernel::Lba)

/// Start After the PM headers, pad 1024 bytes.
#define kNewFSRootCatalogStartAddress (1024)
#define kNewFSCatalogStartAddress	  ((2048) + sizeof(NFS_ROOT_PARTITION_BLOCK))

#define kResourceTypeDialog (10)
#define kResourceTypeString (11)
#define kResourceTypeMenu	(12)

#define kConfigLen (64)
#define kPartLen   (32)

#define kNewFSFlagDeleted	  (70)
#define kNewFSFlagUnallocated (0)
#define kNewFSFlagCreated	  (71)

#define kNewFSMimeNameLen (200)

#define kNewFSForkNameLen (200U)

struct NFS_CATALOG_STRUCT;
struct NFS_FORK_STRUCT;
struct NFS_ROOT_PARTITION_BLOCK;

enum
{
	kNewFSHardDrive			= 0xC0, // Hard Drive
	kNewFSSolidStateDrive	= 0xC1, // Solid State Drive
	kNewFSOpticalDrive		= 0x0C, // Blu-Ray/DVD
	kNewFSMassStorageDevice = 0xCC, // USB
	kNewFSScsi				= 0xC4, // SCSI Hard Drive
	kNewFSFlashDrive		= 0xC6,
	kNewFSUnknown			= 0xFF, // Unknown device.
	kNewFSDriveCount		= 7,
};

/// @brief Catalog type.
struct PACKED NFS_CATALOG_STRUCT final
{
	Kernel::Char Name[kNewFSNodeNameLen];
	Kernel::Char Mime[kNewFSMimeNameLen];

	/// Catalog status flag.
	Kernel::UInt16 Flags;
	/// Custom catalog flags.
	Kernel::UInt16 FileFlags;
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
	Kernel::Char ForkName[kNewFSForkNameLen];
	Kernel::Char CatalogName[kNewFSNodeNameLen];

	Kernel::Int32 Flags;
	Kernel::Int32 Kind;

	Kernel::Int64 ResourceId;
	Kernel::Int32 ResourceKind;
	Kernel::Int32 ResourceFlags;

	Kernel::Lba	  DataOffset; // 8 Where to look for this data?
	Kernel::SizeT DataSize;	  /// Data size according using sector count.

	Kernel::Lba NextSibling;
	Kernel::Lba PreviousSibling;
};

/// @brief Partition block type
struct PACKED NFS_ROOT_PARTITION_BLOCK final
{
	Kernel::Char Ident[kNewFSIdentLen];
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

	Kernel::Char Pad[kNewFSPadLen - sizeof(Kernel::Lba)];
};

namespace Kernel
{
	enum
	{
		kNewFSSubDriveA,
		kNewFSSubDriveB,
		kNewFSSubDriveC,
		kNewFSSubDriveD,
		kNewFSSubDriveInvalid,
		kNewFSSubDriveCount,
	};

	/// \brief Resource fork kind.
	enum
	{
		kNewFSRsrcForkKind = 0,
		kNewFSDataForkKind = 1
	};

	///
	/// \name NewFSParser
	/// \brief NewFS parser class. (catalog creation, remove removal, root,
	/// forks...) Designed like the DOM, detects the filesystem automatically.
	///
	class NewFSParser final
	{
	public:
		explicit NewFSParser() = default;
		~NewFSParser()		   = default;

	public:
		ZKA_COPY_DEFAULT(NewFSParser);

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

		/// @brief Make a EPM+NewFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see ErrLocal().
		bool Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name);

	public:
		Int32 fDriveIndex{kNewFSSubDriveA};
	};

	///
	/// \name NewFilesystemHelper
	/// \brief Filesystem helper and utils.
	///

	class NewFilesystemHelper final
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
