/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <hint/CompilerHint.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>

/// @file Ext2.h
/// @brief EXT2 filesystem structures and constants.

#define kExt2FSMagic			(0xEF53)
#define kExt2FSMaxFileNameLen	(255U)
#define kExt2FSSuperblockOffset (1024)
#define kExt2FSRootInodeNumber	(2)

#define kExt2FSInodeSize	 (128U)
#define kExt2FSBlockSizeBase (1024U)

#define kExt2FSRev0 (0)
#define kExt2FSRev1 (1)

enum
{
	kExt2FileTypeUnknown	  = 0,
	kExt2FileTypeRegular	  = 1,
	kExt2FileTypeDirectory	  = 2,
	kExt2FileTypeCharDevice	  = 3,
	kExt2FileTypeBlockDevice  = 4,
	kExt2FileTypeFIFO		  = 5,
	kExt2FileTypeSocket		  = 6,
	kExt2FileTypeSymbolicLink = 7
};

struct PACKED EXT2_SUPER_BLOCK final
{
	Kernel::UInt32 fInodeCount;
	Kernel::UInt32 fBlockCount;
	Kernel::UInt32 fReservedBlockCount;
	Kernel::UInt32 fFreeBlockCount;
	Kernel::UInt32 fFreeInodeCount;
	Kernel::UInt32 fFirstDataBlock;
	Kernel::UInt32 fLogBlockSize;
	Kernel::UInt32 fLogFragmentSize;
	Kernel::UInt32 fBlocksPerGroup;
	Kernel::UInt32 fFragmentsPerGroup;
	Kernel::UInt32 fInodesPerGroup;
	Kernel::UInt32 fMountTime;
	Kernel::UInt32 fWriteTime;
	Kernel::UInt16 fMountCount;
	Kernel::UInt16 fMaxMountCount;
	Kernel::UInt16 fMagic; // should be 0xEF53
	Kernel::UInt16 fState;
	Kernel::UInt16 fErrors;
	Kernel::UInt16 fMinorRevision;
	Kernel::UInt32 fLastCheck;
	Kernel::UInt32 fCheckInterval;
	Kernel::UInt32 fCreatorOS;
	Kernel::UInt32 fRevisionLevel;
	Kernel::UInt16 fDefaultUID;
	Kernel::UInt16 fDefaultGID;

	// EXT2_DYNAMIC_REV fields
	Kernel::UInt32 fFirstInode;
	Kernel::UInt16 fInodeSize;
	Kernel::UInt16 fBlockGroupNumber;
	Kernel::UInt32 fFeatureCompat;
	Kernel::UInt32 fFeatureIncompat;
	Kernel::UInt32 fFeatureROCompat;
	Kernel::UInt8  fUUID[16];
	Kernel::Char   fVolumeName[16];
	Kernel::Char   fLastMounted[64];
	Kernel::UInt32 fAlgoBitmap;

	// Optional journal fields and padding
	Kernel::UInt8  fPreallocBlocks;
	Kernel::UInt8  fPreallocDirBlocks;
	Kernel::UInt16 fReservedGDTBlocks;

	Kernel::UInt8  fJournalUUID[16];
	Kernel::UInt32 fJournalInode;
	Kernel::UInt32 fJournalDevice;
	Kernel::UInt32 fLastOrphan;

	Kernel::UInt32 fHashSeed[4];
	Kernel::UInt8  fDefHashVersion;
	Kernel::UInt8  fReservedCharPad;
	Kernel::UInt16 fReservedWordPad;
	Kernel::UInt32 fDefaultMountOpts;
	Kernel::UInt32 fFirstMetaBlockGroup;

	Kernel::UInt8 fReserved[760]; // Padding to make 1024 bytes
};

struct PACKED EXT2_INODE final
{
	Kernel::UInt16 fMode;
	Kernel::UInt16 fUID;
	Kernel::UInt32 fSize;
	Kernel::UInt32 fAccessTime;
	Kernel::UInt32 fCreateTime;
	Kernel::UInt32 fModifyTime;
	Kernel::UInt32 fDeleteTime;
	Kernel::UInt16 fGID;
	Kernel::UInt16 fLinksCount;
	Kernel::UInt32 fBlocks;
	Kernel::UInt32 fFlags;
	Kernel::UInt32 fOSD1;

	Kernel::UInt32 fBlock[15]; // 0-11: direct, 12: indirect, 13: double indirect, 14: triple indirect

	Kernel::UInt32 fGeneration;
	Kernel::UInt32 fFileACL;
	Kernel::UInt32 fDirACL; // Only for revision 1+
	Kernel::UInt32 fFragmentAddr;

	Kernel::UInt8 fOSD2[12];
};

struct PACKED EXT2_DIR_ENTRY final
{
	Kernel::UInt32 fInode;
	Kernel::UInt16 fRecordLength;
	Kernel::UInt8  fNameLength;
	Kernel::UInt8  fFileType;
	Kernel::Char   fName[kExt2FSMaxFileNameLen]; // null-terminated, not fixed-length in actual on-disk layout
};