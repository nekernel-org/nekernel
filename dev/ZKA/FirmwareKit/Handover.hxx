/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/**
 * @file Handover.hxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief The handover boot protocol.
 * @version 0.3
 * @date 2024-02-23
 *
 * @copyright Copyright (c) 2024, ZKA Technologies
 *
 */

#pragma once

#include <NewKit/Defines.hxx>

/* useful macros */

#define kHandoverMagic	 0xBADCC
#define kHandoverVersion 0x114

#define kHandoverMaxCmdLine 8

#define kHandoverBetterEFI	 "ZKA_EFI"
#define kHandoverBetterEFI_U L"ZKA_EFI"

#define kHandoverHeapSz gib_cast(2)

#define kHandoverStructSz sizeof(HEL::HandoverInformationHeader)

namespace Kernel::HEL
{
	/**
	@brief the kind of executable we're loading.
*/
	enum
	{
		kTypeKernel		  = 100,
		kTypeKernelDriver = 101,
		kTypeRsrc		  = 102,
		kTypeInvalid	  = 103,
		kTypeCount		  = 4,
	};

	/**
	@brief The executable architecture.
*/

	enum
	{
		kArchAmd64 = 122,
		kArchCount = 2,
	};

	struct HandoverInformationHeader
	{
		UInt64 f_Magic;
		UInt64 f_Version;

		VoidPtr f_VirtualStart;
		SizeT	f_VirtualSize;
		VoidPtr f_PhysicalStart;
		VoidPtr f_HeapStart;

		VoidPtr f_KernelImage;
		VoidPtr f_StartupChime;
		VoidPtr f_StartupImage;
		VoidPtr f_TTFallbackFont;

		WideChar f_FirmwareVendorName[32];
		SizeT	 f_FirmwareVendorLen;

		VoidPtr f_FirmwareCustomTables[2]; // On EFI 0: BS 1: ST

		struct
		{
			VoidPtr f_SmBios;
			VoidPtr f_VendorPtr;
			VoidPtr f_MpPtr;
			Bool	f_MultiProcessingEnabled;
		} f_HardwareTables;

		struct
		{
			UIntPtr f_The;
			SizeT	f_Size;
			UInt32	f_Width;
			UInt32	f_Height;
			UInt32	f_PixelFormat;
			UInt32	f_PixelPerLine;
		} f_GOP;

		UInt64 f_FirmwareSpecific[8];
	};

	enum
	{
		kHandoverSpecificKind,
		kHandoverSpecificAttrib,
		kHandoverSpecificMemoryEfi,
	};

	/// @brief Bootloader main type.
	typedef void (*BootMainKind)(HandoverInformationHeader* handoverInfo);

	/// @brief Alias of bootloader main type.
	typedef void (*HandoverProc)(HandoverInformationHeader* handoverInfo);
} // namespace Kernel::HEL

/// @brief Bootloader global header.
inline Kernel::HEL::HandoverInformationHeader* kHandoverHeader = nullptr;
