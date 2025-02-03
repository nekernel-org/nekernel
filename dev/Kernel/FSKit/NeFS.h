/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	FILE: NeFS.h
	PURPOSE: NeFS (New extended File System) support.

	Revision History:

	?/?/?: Added file (amlel)
	12/02/24: Add UUID macro for EPM and GPT partition schemes.
	3/16/24: Add mandatory sector size, kNeFSSectorSz is set to 2048 by
default.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <Hints/CompilerHint.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>

/**
	@brief New extended File System specification.
	@author Amlal EL Mahrouss (Amlal EL Mahrouss, amlalelmahrouss at icloud dot com)
*/

#define kNeFSInvalidFork	(-1)
#define kNeFSInvalidCatalog (-1)
#define kNeFSNodeNameLen	(256)

#define kNeFSMinimumDiskSize (gib_cast(4))

#define kNeFSSectorSz (512)

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
#define kNeFSCatalogKindMPEG (15)
#define kNeFSCatalogKindMOFF (16)

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

#define kNeFSFrameworkExt	".fwrk"
#define kNeFSStepsExt		".step"
#define kNeFSApplicationExt ".app"
#define kNeFSJournalExt		".jrnl"

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

enum
{
	kNeFSStatusUnlocked = 0x18,
	kNeFSStatusLocked,
	kNeFSStatusError,
	kNeFSStatusInvalid,
};

/// @brief Catalog type.
struct PACKED NFS_CATALOG_STRUCT final
{
	BOOL ForkOrCatalog : 1 {0};

	Kernel::Char Name[kNeFSNodeNameLen] = {0};
	Kernel::Char Mime[kNeFSMimeNameLen] = {0};

	/// Catalog flags.
	Kernel::UInt16 Flags;

	/// Catalog allocation status.
	Kernel::UInt16 Status;

	/// Custom catalog flags.
	Kernel::UInt16 CatalogFlags;

	/// Catalog kind.
	Kernel::Int32 Kind;

	/// Size of the data fork.
	Kernel::Lba DataForkSize;

	/// Size of all resource forks.
	Kernel::Lba ResourceForkSize;

	/// Forks LBA.
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
	BOOL ForkOrCatalog : 1 {1};

	Kernel::Char ForkName[kNeFSForkNameLen]	   = {0};
	Kernel::Char CatalogName[kNeFSNodeNameLen] = {0};

	Kernel::Int32 Flags;
	Kernel::Int32 Kind;

	Kernel::Int64 ResourceId;
	Kernel::Int32 ResourceKind;
	Kernel::Int32 ResourceFlags;

	Kernel::Lba	  DataOffset; // 8 Where to look for this data?
	Kernel::SizeT DataSize;	  /// Data size according using sector count.

	Kernel::Lba NextSibling;
	Kernel::Lba PreviousSibling;

	Kernel::Char Pad[2] = {0};
};

/// @brief Partition block type
struct PACKED NFS_ROOT_PARTITION_BLOCK final
{
	Kernel::Char Ident[kNeFSIdentLen]	 = {0};
	Kernel::Char PartitionName[kPartLen] = {0};

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
	class NeFileSystemParser;
	class NeFileSystemJournal;
	class NeFileSystemHelper;

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
	/// \name NeFileSystemParser
	/// \brief NeFS parser class. (catalog creation, remove removal, root,
	/// forks...) Designed like the DOM, detects the filesystem automatically.
	///
	class NeFileSystemParser final
	{
	public:
		explicit NeFileSystemParser() = default;
		~NeFileSystemParser()		  = default;

	public:
		NE_COPY_DEFAULT(NeFileSystemParser);

	public:
		/// @brief Creates a new fork inside the NeFS  partition.
		/// @param catalog it's catalog
		/// @param theFork the fork itself.
		/// @return the fork
		_Output BOOL CreateFork(_Input NFS_FORK_STRUCT& in);

		/// @brief Find fork inside New filesystem.
		/// @param catalog the catalog.
		/// @param name the fork name.
		/// @return the fork.
		_Output NFS_FORK_STRUCT* FindFork(_Input NFS_CATALOG_STRUCT* catalog,
										  _Input const Char* name,
										  Boolean			 data);

		_Output Void RemoveFork(_Input NFS_FORK_STRUCT* fork);

		_Output Void CloseFork(_Input NFS_FORK_STRUCT* fork);

		_Output NFS_CATALOG_STRUCT* FindCatalog(_Input const Char* catalog_name, Lba& ou_lba, Bool search_hidden = YES, Bool local_search = NO);

		_Output NFS_CATALOG_STRUCT* GetCatalog(_Input const Char* name);

		_Output NFS_CATALOG_STRUCT* CreateCatalog(_Input const Char* name,
												  _Input const Int32& flags,
												  _Input const Int32& kind);

		_Output NFS_CATALOG_STRUCT* CreateCatalog(_Input const Char* name);

		_Output Bool WriteCatalog(_Input const Char* catalog,
								  _Input Bool		 rsrc,
								  _Input VoidPtr	 data,
								  _Input SizeT		 sz,
								  _Input const Char* name);

		_Output VoidPtr ReadCatalog(_Input _Output NFS_CATALOG_STRUCT* catalog,
									_Input Bool						   isRsrcFork,
									_Input SizeT					   dataSz,
									_Input const Char* forkName);

		_Output Bool Seek(_Input _Output NFS_CATALOG_STRUCT* catalog, SizeT off);

		_Output SizeT Tell(_Input _Output NFS_CATALOG_STRUCT* catalog);

		_Output Bool RemoveCatalog(_Input const Char* catalog);

		_Output Bool CloseCatalog(_InOut NFS_CATALOG_STRUCT* catalog);

		/// @brief Make a EPM+NeFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see err_local_get().
		_Output Bool Format(_Input _Output DriveTrait* drive, _Input const Lba endLba, _Input const Int32 flags, const Char* part_name);

	public:
		UInt32 mDriveIndex{kNeFSSubDriveA};
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

	/// @brief Journal class for NeFS.
	class NeFileSystemJournal final
	{
	private:
		NFS_CATALOG_STRUCT* mNode{nullptr};

	public:
		explicit NeFileSystemJournal(const char* stamp = nullptr)
		{
			if (!stamp)
			{
				kcout << "Invalid: Journal Stamp, using default name.\r";
				return;
			}

			kcout << "Info: Journal stamp: " << stamp << endl;
			rt_copy_memory((VoidPtr)stamp, this->mStamp, rt_string_len(stamp));
		}

		~NeFileSystemJournal() = default;

		NE_COPY_DEFAULT(NeFileSystemJournal);

		Bool CreateJournal(NeFileSystemParser* parser)
		{
			if (!parser)
				return NO;

			delete parser->CreateCatalog("/etc/xml/", 0, kNeFSCatalogKindDir);
			mNode = parser->CreateCatalog(mStamp);

			if (!mNode)
				return NO;

			return YES;
		}

		Bool GetJournal(NeFileSystemParser* parser)
		{
			if (!parser)
				return NO;

			auto node = parser->GetCatalog(mStamp);

			if (node)
			{
				mNode = node;
				return YES;
			}

			return NO;
		}

		Bool ReleaseJournal()
		{
			if (mNode)
			{
				delete mNode;
				mNode = nullptr;
				return YES;
			}

			return NO;
		}

		Bool CommitJournal(NeFileSystemParser* parser,
						   Char*			   xml_data,
						   Char*			   journal_name)
		{
			if (!parser ||
				!mNode)
				return NO;

			NFS_FORK_STRUCT new_fork{};

			rt_copy_memory(mStamp, new_fork.CatalogName, rt_string_len(mStamp));
			rt_copy_memory(journal_name, new_fork.ForkName, rt_string_len(journal_name));

			new_fork.ResourceKind  = 0;
			new_fork.ResourceId	   = 0;
			new_fork.ResourceFlags = 0;
			new_fork.DataSize	   = rt_string_len(xml_data);
			new_fork.Kind		   = kNeFSRsrcForkKind;

			if (!parser->CreateFork(new_fork))
				return NO;

			kcout << "XML Commited: " << xml_data << "\r\nTo Journal Fork: " << journal_name << endl;

			auto ret = parser->WriteCatalog(new_fork.CatalogName, YES, xml_data, rt_string_len(xml_data), new_fork.ForkName);

			return ret;
		}

	private:
		Char mStamp[kNeFSNodeNameLen] = {"/etc/xml/journal" kNeFSJournalExt};
	};

	namespace NeFS
	{
		Boolean fs_init_nefs(Void) noexcept;
	} // namespace NeFS
} // namespace Kernel
