/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file BootKit.h
/// @brief Bootloader Application Programming Interface.
/***********************************************************************************/

#pragma once

#include <BootKit/HW/ATA.h>
#include <CompilerKit/Version.h>
#include <modules/CoreGfx/CoreGfx.h>

/// include NeFS header and Support header as well.

#include <FSKit/NeFS.h>
#include <BootKit/Support.h>

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <NewKit/Defines.h>
#include <modules/ATA/ATA.h>

#include <FirmwareKit/VEPM.h>
#include <FirmwareKit/EPM.h>
#include <FirmwareKit/GPT.h>
#include <FirmwareKit/EFI.h>

/***********************************************************************************/
/// Framebuffer helpers.
/***********************************************************************************/

namespace Boot
{
	EXTERN void ThrowError(const WideChar* errorCode,
						   const WideChar* reason) noexcept;

	class BootTextWriter;
	class BootFileReader;
	class BootThread;
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
	class BootTextWriter final
	{
		BootTextWriter& _Write(const UInt64& num);

	public:
		BootTextWriter& Write(const UInt64& num);
		BootTextWriter& Write(const Char* str);
		BootTextWriter& Write(const CharacterTypeUTF16* str);
		BootTextWriter& WriteCharacter(CharacterTypeUTF16 c);
		BootTextWriter& Write(const UChar* str);

		template <typename T>
		BootTextWriter& operator<<(T elem)
		{
			this->Write(elem);
			return *this;
		}

	public:
		explicit BootTextWriter() = default;
		~BootTextWriter()		  = default;

	public:
		BootTextWriter& operator=(const BootTextWriter&) = default;
		BootTextWriter(const BootTextWriter&)			 = default;
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
	class BootFileReader final
	{
	public:
		explicit BootFileReader(const CharacterTypeUTF16* path,
								EfiHandlePtr			  ImageHandle);
		~BootFileReader();

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
		BootFileReader& operator=(const BootFileReader&) = default;
		BootFileReader(const BootFileReader&)			 = default;

	private:
		Int32			   mErrorCode{kOperationOkay};
		VoidPtr			   mBlob{nullptr};
		CharacterTypeUTF16 mPath[kPathLen];
		BootTextWriter	   mWriter;
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
			Char  fFileName[kNeFSCatalogNameLen];
			Int32 fKind;
		};

	public:
		explicit BDiskFormatFactory() = default;
		explicit BDiskFormatFactory(BootDev dev)
			: fDiskDev(dev)
		{
		}

		~BDiskFormatFactory() = default;

		NE_COPY_DELETE(BDiskFormatFactory)

		/// @brief Format disk using partition name and blob.
		/// @param Partition part_name the target partition name.
		/// @param blob blobs array.
		/// @param blob_sz blobs array count.
		/// @retval True disk has been formatted.
		/// @retval False failed to format.
		Boolean Format(const Char* part_name, BFileDescriptor* blob, SizeT blob_sz);

		/// @brief check if partition is good.
		Bool IsPartitionValid() noexcept
		{
#if defined(BOOTZ_EPM_SUPPORT)
			fDiskDev.Leak().mBase = (kNeFSRootCatalogStartAddress);
			fDiskDev.Leak().mSize = BootDev::kSectorSize;

			Char buf[BootDev::kSectorSize] = {0};

			fDiskDev.Read(buf, BootDev::kSectorSize);

			NEFS_ROOT_PARTITION_BLOCK* blockPart = reinterpret_cast<NEFS_ROOT_PARTITION_BLOCK*>(buf);

			BootTextWriter writer;

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

			writer.Write(L"BootZ: Partition: ").Write(blockPart->PartitionName).Write(L" is healthy.\r");

			return true;
#else
			return false;
#endif
		}

	private:
		/// @brief Write all of the requested catalogs into the filesystem.
		/// @param blob the blobs.
		/// @param blob_sz the number of blobs to write (n * sizeof(blob_struct)).
		/// @param part the NeFS partition block.
		Boolean WriteCatalogList(BFileDescriptor* blob, SizeT blob_sz, NEFS_ROOT_PARTITION_BLOCK& part)
		{
			if (blob_sz < sizeof(NEFS_CATALOG_STRUCT))
				return NO;

			if (!blob)
				return NO;

			Lba			   startLba = part.StartCatalog;
			BootTextWriter writer;

			NEFS_CATALOG_STRUCT catalogKind{0};

			constexpr auto cNeFSCatalogPadding = 4;

			catalogKind.PrevSibling = startLba;
			catalogKind.NextSibling = (startLba + sizeof(NEFS_CATALOG_STRUCT) * cNeFSCatalogPadding);

			/// Fill catalog kind.
			catalogKind.Kind = blob->fKind;
			catalogKind.Flags |= kNeFSFlagCreated;
			catalogKind.CatalogFlags = kNeFSStatusUnlocked;

			--part.FreeCatalog;
			--part.FreeSectors;

			CopyMem(catalogKind.Name, blob->fFileName, StrLen(blob->fFileName));

			fDiskDev.Leak().mBase = startLba;
			fDiskDev.Leak().mSize = sizeof(NEFS_CATALOG_STRUCT);

			fDiskDev.Write((Char*)&catalogKind, sizeof(NEFS_CATALOG_STRUCT));

			writer.Write(L"BootZ: wrote root directory: ").Write(blob->fFileName).Write(L"\r");

			return true;
		}

	private:
		BootDev fDiskDev;
	};

	/// @brief Format disk with a specific partition scheme.
	/// @param part_name partition Name
	/// @param blob blos.
	/// @param blob_sz n blobs (n *  sizeof(blob_struct)).
	/// @retval True disk has been formatted.
	/// @retval False failed to format.
	template <typename BootDev>
	inline Boolean BDiskFormatFactory<BootDev>::Format(const Char*							part_name,
													   BDiskFormatFactory::BFileDescriptor* blob,
													   SizeT								blob_sz)
	{
		if (!blob || !blob_sz)
			return false; /// sanity check

		/// @note A catalog roughly equal to a sector in NeFS terms.
		constexpr auto kMinimumDiskSize = kNeFSMinimumDiskSize; // at minimum.

		/// @note also look at EPM headers, for free part blocks. (only applies if EPM or vEPM is used)

		if (fDiskDev.GetDiskSize() < kMinimumDiskSize)
		{
			Boot::ThrowError(L"Drive-Too-Tiny", L"Can't format a NeFS partition here.");
			return false;
		}

		NEFS_ROOT_PARTITION_BLOCK part{};

		CopyMem(part.Ident, kNeFSIdent, kNeFSIdentLen - 1);
		CopyMem(part.PartitionName, part_name, StrLen(part_name));

		part.Version	  = kNeFSVersionInteger;
		part.CatalogCount = blob_sz / sizeof(NEFS_CATALOG_STRUCT);
		part.Kind		  = BootDev::kSectorSize;
		part.SectorSize	  = kATASectorSize;
		part.FreeCatalog  = fDiskDev.GetSectorsCount() / sizeof(NEFS_CATALOG_STRUCT);
		part.SectorCount  = fDiskDev.GetSectorsCount();
		part.FreeSectors  = fDiskDev.GetSectorsCount();
		part.StartCatalog = kNeFSCatalogStartAddress;
		part.DiskSize	  = fDiskDev.GetDiskSize();
		part.Flags		  = kNeFSPartitionTypeBoot | kNeFSPartitionTypeStandard;

		BootTextWriter writer;

		writer << "BootZ: Partition name: " << part.PartitionName << "\r";
		writer << "BootZ: Start: " << part.StartCatalog << "\r";
		writer << "BootZ: Number of catalogs: " << part.CatalogCount << "\r";
		writer << "BootZ: Free catalog: " << part.FreeCatalog << "\r";
		writer << "BootZ: Free sectors: " << part.FreeSectors << "\r";
		writer << "BootZ: Sector size: " << part.SectorSize << "\r";

#if defined(BOOTZ_EPM_SUPPORT)
		EPM_PART_BLOCK epm_boot{};

		const auto kFsName	  = "NeFS";
		const auto kBlockName = "OS (EPM)";

		epm_boot.FsVersion = kNeFSVersionInteger;
		epm_boot.LbaStart  = kNeFSRootCatalogStartAddress;
		epm_boot.LbaEnd	   = fDiskDev.GetDiskSize();
		epm_boot.SectorSz  = part.SectorSize;
		epm_boot.Kind	   = kEPMNeKernel;
		epm_boot.NumBlocks = part.CatalogCount;

		epm_boot.Guid = kEPMNilGuid;

		CopyMem(epm_boot.Fs, reinterpret_cast<VoidPtr>(const_cast<Char*>(kFsName)), StrLen(kFsName));
		CopyMem(epm_boot.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), StrLen(kBlockName));
		CopyMem(epm_boot.Magic, reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), StrLen(kEPMMagic));

		fDiskDev.Leak().mBase = kEPMBootBlockLba; // always always resies at zero block.
		fDiskDev.Leak().mSize = sizeof(EPM_PART_BLOCK);

		fDiskDev.Write((Char*)&epm_boot, sizeof(EPM_PART_BLOCK));

		fDiskDev.Leak().mBase = kNeFSRootCatalogStartAddress;
		fDiskDev.Leak().mSize = sizeof(NEFS_ROOT_PARTITION_BLOCK);

		fDiskDev.Write((Char*)&part, sizeof(NEFS_ROOT_PARTITION_BLOCK));

		writer.Write(L"BootZ: Drive is EPM formatted.\r");
#elif defined(BOOTZ_GPT_SUPPORT) || defined(BOOTZ_VEPM_SUPPORT)
		GPT_PARTITION_TABLE gpt_part{};

		CopyMem(gpt_part.Signature, reinterpret_cast<VoidPtr>(const_cast<Char*>(kMagicGPT)), StrLen(kMagicGPT));

		gpt_part.Revision = 0x00010000;
		gpt_part.HeaderSize = sizeof(GPT_PARTITION_TABLE);

		gpt_part.CRC32 = 0x00000000;

		gpt_part.Reserved1 = 0x00000000;
		gpt_part.LBAHeader = 0x00000000;
		gpt_part.LBAAltHeader = 0x00000000;
		gpt_part.FirstGPTEntry = 0x00000000;
		gpt_part.LastGPTEntry = 0x00000000;

#if defined(BOOTZ_GPT_SUPPORT)
		gpt_part.Guid.Data1 = 0x00000000;
		gpt_part.Guid.Data2 = 0x0000;
		gpt_part.Guid.Data3 = 0x0000;

		SetMem(gpt_part.Guid.Data4, 0, 8);
#else
		gpt_part.Guid		  = kVEPMGuidEFI;
#endif

		gpt_part.Revision = 0x00010000;

		gpt_part.StartingLBA = 0x00000000;
		gpt_part.NumPartitionEntries = 0x00000000;
		gpt_part.SizeOfEntries = 0x00000000;
		gpt_part.CRC32PartEntry = 0x00000000;

		SetMem(gpt_part.Reserved2, 0, kSectorAlignGPT_PartTbl);

		fDiskDev.Leak().mBase = kGPTPartitionTableLBA; // always always resies at zero block.
		fDiskDev.Leak().mSize = sizeof(GPT_PARTITION_TABLE);

		fDiskDev.Write((Char*)&gpt_part, sizeof(GPT_PARTITION_TABLE));

#if defined(BOOTZ_VEPM_SUPPORT)
		const auto kBlockName = "OS (VEPM)";

		GPT_PARTITION_ENTRY gpt_part_entry{};

		gpt_part_entry.StartLBA = kNeFSRootCatalogStartAddress;
		gpt_part_entry.EndLBA = fDiskDev.GetDiskSize();
		gpt_part_entry.Attributes = 0x00000000;

		gpt_part_entry.PartitionTypeGUID.Data1 = 0x00000000;
		gpt_part_entry.PartitionTypeGUID.Data2 = 0x0000;
		gpt_part_entry.PartitionTypeGUID.Data3 = 0x0000;

		CopyMem(gpt_part_entry.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), StrLen(kBlockName));

		SetMem(gpt_part_entry.PartitionTypeGUID.Data4, 0, 8);

		EPM_PART_BLOCK epm_boot{};

		const auto kFsName = "NeFS";

		epm_boot.FsVersion = kNeFSVersionInteger;
		epm_boot.LbaStart = kNeFSRootCatalogStartAddress;
		epm_boot.LbaEnd = fDiskDev.GetDiskSize();
		epm_boot.SectorSz = part.SectorSize;
		epm_boot.Kind = kEPMNeKernel;
		epm_boot.NumBlocks = part.CatalogCount;

		epm_boot.Guid = kEPMNilGuid;

		CopyMem(epm_boot.Fs, reinterpret_cast<VoidPtr>(const_cast<Char*>(kFsName)), StrLen(kFsName));
		CopyMem(epm_boot.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), StrLen(kBlockName));
		CopyMem(epm_boot.Magic, reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), StrLen(kEPMMagic));

		fDiskDev.Leak().mBase = kGPTPartitionTableLBA + sizeof(GPT_PARTITION_TABLE);
		fDiskDev.Leak().mSize = sizeof(GPT_PARTITION_ENTRY);

		fDiskDev.Write((Char*)&gpt_part_entry, sizeof(GPT_PARTITION_ENTRY));

		fDiskDev.Leak().mBase = gpt_part_entry.StartLBA;
		fDiskDev.Leak().mSize = sizeof(EPM_PART_BLOCK);

		fDiskDev.Write((Char*)&epm_boot, sizeof(EPM_PART_BLOCK));

		fDiskDev.Leak().mBase = gpt_part_entry.StartLBA + kNeFSRootCatalogStartAddress;
		fDiskDev.Leak().mSize = sizeof(NEFS_ROOT_PARTITION_BLOCK);

		fDiskDev.Write((Char*)&part, sizeof(NEFS_ROOT_PARTITION_BLOCK));

		writer.Write(L"BootZ: Drive is vEPM formatted.\r");
#else
		const auto kBlockName = "OS (GPT)";

		GPT_PARTITION_ENTRY gpt_part_entry{0};

		gpt_part_entry.StartLBA	  = kNeFSRootCatalogStartAddress;
		gpt_part_entry.EndLBA	  = fDiskDev.GetDiskSize();
		gpt_part_entry.Attributes = 0x00000000;

		gpt_part_entry.PartitionTypeGUID.Data1 = 0x00000000;
		gpt_part_entry.PartitionTypeGUID.Data2 = 0x0000;
		gpt_part_entry.PartitionTypeGUID.Data3 = 0x0000;

		CopyMem(gpt_part_entry.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)), StrLen(kBlockName));

		SetMem(gpt_part_entry.PartitionTypeGUID.Data4, 0, 8);

		fDiskDev.Leak().mBase = kGPTPartitionTableLBA + sizeof(GPT_PARTITION_TABLE);
		fDiskDev.Leak().mSize = sizeof(GPT_PARTITION_ENTRY);
		fDiskDev.Write((Char*)&gpt_part_entry, sizeof(GPT_PARTITION_ENTRY));

		fDiskDev.Leak().mBase = gpt_part_entry.StartLBA;
		fDiskDev.Leak().mSize = sizeof(NEFS_ROOT_PARTITION_BLOCK);

		fDiskDev.Write((Char*)&part, sizeof(NEFS_ROOT_PARTITION_BLOCK));

		writer.Write(L"BootZ: Drive is GPT formatted.\r");
#endif /// defined(BOOTZ_VEPM_SUPPORT)
#endif

		return YES;
	}
} // namespace Boot
