/* -------------------------------------------

	Copyright (C) 2024, ELMH GROUP, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file BootKit.h
/// @brief Bootloader Application Programming Interface.
/***********************************************************************************/

#pragma once

#include <BootKit/HW/ATA.h>
#include <FirmwareKit/EPM.h>
#include <CompilerKit/Version.h>

/// include NeFS header and Support header as well.

#include <FSKit/NeFS.h>
#include <BootKit/Support.h>

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <NewKit/Defines.h>
#include <Modules/ATA/ATA.h>

#include <FirmwareKit/EFI.h>

/***********************************************************************************/
/// Framebuffer helpers.
/***********************************************************************************/

namespace EFI
{
	EXTERN void ThrowError(const WideChar* errorCode,
						   const WideChar* reason) noexcept;
} // namespace EFI

namespace Boot
{
	class BTextWriter;
	class BFileReader;
	class BThread;
	class BVersionString;

	typedef Char* PEFImagePtr;
	typedef Char* PEImagePtr;

	typedef WideChar CharacterTypeUTF16;
	typedef Char	 CharacterTypeUTF8;

	using namespace Kernel;

	/**
	 * @brief BootKit Text Writer class
	 * Writes to UEFI StdOut.
	 */
	class BTextWriter final
	{
		BTextWriter& _Write(const Long& num);

	public:
		BTextWriter& Write(const Long& num);
		BTextWriter& Write(const Char* str);
		BTextWriter& Write(const CharacterTypeUTF16* str);
		BTextWriter& WriteCharacter(CharacterTypeUTF16 c);
		BTextWriter& Write(const UChar* str);

	public:
		explicit BTextWriter() = default;
		~BTextWriter()		   = default;

	public:
		BTextWriter& operator=(const BTextWriter&) = default;
		BTextWriter(const BTextWriter&)			   = default;
	};

	Kernel::SizeT BCopyMem(CharacterTypeUTF16* dest, CharacterTypeUTF16* src, const Kernel::SizeT len);

	Kernel::SizeT BSetMem(CharacterTypeUTF8* src, const CharacterTypeUTF8 byte, const Kernel::SizeT len);

	/// String length functions.

	/// @brief get string length.
	Kernel::SizeT BStrLen(const CharacterTypeUTF16* ptr);

	/// @brief set memory with custom value.
	Kernel::SizeT BSetMem(CharacterTypeUTF16* src, const CharacterTypeUTF16 byte, const Kernel::SizeT len);

	/**
	 * @brief BootKit File Reader class
	 * Reads the Firmware Boot partition and filesystem.
	 */
	class BFileReader final
	{
	public:
		explicit BFileReader(const CharacterTypeUTF16* path,
							 EfiHandlePtr			   ImageHandle);
		~BFileReader();

	public:
		Void ReadAll(SizeT until, SizeT chunk = kib_cast(4), UIntPtr out_address = 0UL);

		enum
		{
			kOperationOkay,
			kNotSupported,
			kEmptyDirectory,
			kNoSuchEntry,
			kIsDirectory,
			kTooSmall,
			kCount,
		};

		/// @brief error code getter.
		/// @return the error code.
		Int32& Error();

		/// @brief blob getter.
		/// @return the blob.
		VoidPtr Blob();

		/// @breif Size getter.
		/// @return the size of the file.
		UInt64& Size();

	public:
		BFileReader& operator=(const BFileReader&) = default;
		BFileReader(const BFileReader&)			   = default;

	private:
		Int32			   mErrorCode{kOperationOkay};
		VoidPtr			   mBlob{nullptr};
		CharacterTypeUTF16 mPath[kPathLen];
		BTextWriter		   mWriter;
		EfiFileProtocol*   mFile{nullptr};
		UInt64			   mSizeFile{0};
		EfiFileProtocol*   mRootFs;
	};

	typedef UInt8* BlobType;

	class BVersionString final
	{
	public:
		static const CharacterTypeUTF8* The()
		{
			return BOOTLOADER_VERSION;
		}
	};

	/***********************************************************************************/
	/// Provide some useful processor features.
	/***********************************************************************************/

#ifdef __EFI_x86_64__

	/***
	 * Common processor instructions.
	 */

	EXTERN_C void	rt_out8(UInt16 port, UInt8 value);
	EXTERN_C void	rt_out16(UInt16 port, UInt16 value);
	EXTERN_C void	rt_out32(UInt16 port, UInt32 value);
	EXTERN_C UInt8	rt_in8(UInt16 port);
	EXTERN_C UInt16 In16(UInt16 port);
	EXTERN_C UInt32 rt_in32(UInt16 port);

	EXTERN_C void rt_hlt();
	EXTERN_C void rt_cli();
	EXTERN_C void rt_sti();
	EXTERN_C void rt_cld();
	EXTERN_C void rt_std();

#endif // __EFI_x86_64__

	static inline const UInt32 kRgbRed	 = 0x000000FF;
	static inline const UInt32 kRgbGreen = 0x0000FF00;
	static inline const UInt32 kRgbBlue	 = 0x00FF0000;
	static inline const UInt32 kRgbBlack = 0x00000000;
	static inline const UInt32 kRgbWhite = 0x00FFFFFF;

#define kBKBootFileMime "boot-x/file"
#define kBKBootDirMime	"boot-x/dir"

	/// @brief BootKit Drive Formatter.
	template <typename BootDev>
	class BDiskFormatFactory final
	{
	public:
		/// @brief File entry for **BDiskFormatFactory**.
		struct BFileDescriptor final
		{
			Char  fFileName[kNeFSNodeNameLen];
			Int32 fKind;
		};

	public:
		explicit BDiskFormatFactory() = default;
		explicit BDiskFormatFactory(BootDev dev)
			: fDiskDev(dev)
		{
		}

		~BDiskFormatFactory() = default;

		ZKA_COPY_DELETE(BDiskFormatFactory);

		/// @brief Format disk using partition name and fileBlobs.
		/// @param Partition partName the target partition name.
		/// @param fileBlobs blobs array.
		/// @param blobCount blobs array count.
		/// @retval True disk has been formatted.
		/// @retval False failed to format.
		Boolean Format(const Char* partName, BFileDescriptor* fileBlobs, SizeT blobCount);

		/// @brief check if partition is good.
		Bool IsPartitionValid() noexcept
		{
			fDiskDev.Leak().mBase = (kNeFSRootCatalogStartAddress);
			fDiskDev.Leak().mSize = BootDev::kSectorSize;

			Char buf[BootDev::kSectorSize] = {0};

			fDiskDev.Read(buf, BootDev::kSectorSize);

			NFS_ROOT_PARTITION_BLOCK* blockPart = reinterpret_cast<NFS_ROOT_PARTITION_BLOCK*>(buf);

			BTextWriter writer;

			for (SizeT indexMag = 0UL; indexMag < kNeFSIdentLen; ++indexMag)
			{
				if (blockPart->Ident[indexMag] != kNeFSIdent[indexMag])
					return false;
			}

			if (blockPart->DiskSize != this->fDiskDev.GetDiskSize() ||
				blockPart->DiskSize < 1 ||
				blockPart->SectorSize != BootDev::kSectorSize ||
				blockPart->Version != kNeFSVersionInteger ||
				blockPart->StartCatalog == 0)
			{
				return false;
			}
			else if (blockPart->PartitionName[0] == 0)
			{
				return false;
			}

			writer.Write(L"ZBA: Partition: ").Write(blockPart->PartitionName).Write(L" is healthy.\r");

			return true;
		}

	private:
		/// @brief Write all of the requested catalogs into the filesystem.
		/// @param fileBlobs the blobs.
		/// @param blobCount the number of blobs to write.
		/// @param partBlock the NeFS partition block.
		Boolean WriteRootCatalog(BFileDescriptor* fileBlobs, SizeT blobCount, NFS_ROOT_PARTITION_BLOCK& partBlock)
		{
			BFileDescriptor* blob	  = fileBlobs;
			Lba				 startLba = partBlock.StartCatalog;
			BTextWriter		 writer;

			NFS_CATALOG_STRUCT catalogKind{0};

			constexpr auto cNeFSCatalogPadding = 4;

			catalogKind.PrevSibling = startLba;
			catalogKind.NextSibling = (startLba + sizeof(NFS_CATALOG_STRUCT) * cNeFSCatalogPadding);

			/// Fill catalog kind.
			catalogKind.Kind  = blob->fKind;
			catalogKind.Flags = kNeFSFlagCreated;

			--partBlock.FreeCatalog;
			--partBlock.FreeSectors;

			CopyMem(catalogKind.Name, blob->fFileName, StrLen(blob->fFileName));

			fDiskDev.Leak().mBase = startLba;
			fDiskDev.Leak().mSize = sizeof(NFS_CATALOG_STRUCT);

			fDiskDev.Write((Char*)&catalogKind, sizeof(NFS_CATALOG_STRUCT));

			writer.Write(L"ZBA: Wrote directory: ").Write(blob->fFileName).Write(L"\r");

			return true;
		}

	private:
		BootDev fDiskDev;
	};

	/// @brief Format disk.
	/// @param Partition Name
	/// @param Blobs.
	/// @param Number of blobs.
	/// @retval True disk has been formatted.
	/// @retval False failed to format.
	template <typename BootDev>
	inline Boolean BDiskFormatFactory<BootDev>::Format(const Char*							partName,
													   BDiskFormatFactory::BFileDescriptor* fileBlobs,
													   SizeT								blobCount)
	{
		if (!fileBlobs || !blobCount)
			return false; /// sanity check

		/// convert the sector into something that the disk understands.
		SizeT sectorSz = BootDev::kSectorSize;

		/// @note A catalog roughly equal to a sector.

		constexpr auto cMinimumDiskSize = kNeFSMinimumDiskSize; // at minimum.

		/// @note also look at EPM headers, for free part blocks.

		if (fDiskDev.GetDiskSize() < cMinimumDiskSize)
		{
			EFI::ThrowError(L"Drive-Too-Tiny", L"Can't format a New Filesystem partition here.");
			return false;
		}

		NFS_ROOT_PARTITION_BLOCK partBlock{0};

		CopyMem(partBlock.Ident, kNeFSIdent, kNeFSIdentLen - 1);
		CopyMem(partBlock.PartitionName, partName, strlen(partName));

		partBlock.Version	   = kNeFSVersionInteger;
		partBlock.CatalogCount = blobCount;
		partBlock.Kind		   = kNeFSHardDrive;
		partBlock.SectorSize   = sectorSz;
		partBlock.FreeCatalog  = fDiskDev.GetSectorsCount() / sizeof(NFS_CATALOG_STRUCT);
		partBlock.SectorCount  = fDiskDev.GetSectorsCount();
		partBlock.FreeSectors  = fDiskDev.GetSectorsCount();
		partBlock.StartCatalog = kNeFSCatalogStartAddress;
		partBlock.DiskSize	   = fDiskDev.GetDiskSize();
		partBlock.Flags		   = kNeFSPartitionTypeBoot | kNeFSPartitionTypeStandard;

		fDiskDev.Leak().mBase = kNeFSRootCatalogStartAddress;
		fDiskDev.Leak().mSize = sectorSz;

		fDiskDev.Write((Char*)&partBlock, sectorSz);

		BOOT_BLOCK_STRUCT epmBoot{0};

		constexpr auto cFsName	  = "NeFS";
		constexpr auto cBlockName = "ZKA:";

		CopyMem(epmBoot.Fs, reinterpret_cast<VoidPtr>(const_cast<Char*>(cFsName)), StrLen(cFsName));

		epmBoot.FsVersion = kNeFSVersionInteger;
		epmBoot.LbaStart  = kNeFSRootCatalogStartAddress;
		epmBoot.SectorSz  = partBlock.SectorSize;
		epmBoot.Kind	  = kEPMNewOS;
		epmBoot.NumBlocks = partBlock.CatalogCount;

		CopyMem(epmBoot.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(cBlockName)), StrLen(cBlockName));
		CopyMem(epmBoot.Magic, reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), StrLen(kEPMMagic));

		fDiskDev.Leak().mBase = 1; // always always resies at zero block.
		fDiskDev.Leak().mSize = BootDev::kSectorSize;

		fDiskDev.Write((Char*)&epmBoot, sectorSz);

		/// if we can write a root catalog, then write the partition block.
		if (this->WriteRootCatalog(fileBlobs, blobCount, partBlock))
		{
			BTextWriter writer;
			writer.Write(L"ZBA: Drive formatted.\r");

			return true;
		}
		else
		{
			EFI::ThrowError(L"Filesystem-Failure-Part", L"Filesystem couldn't be partitioned, this drive cannot be formatted as an explicit partition map.");
		}

		return false;
	}
} // namespace Boot
