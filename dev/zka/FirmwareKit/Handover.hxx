/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

/**
 * @file Handover.hxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief The handover boot protocol.
 * @version 1.15
 * @date 2024-02-23
 *
 * @copyright Copyright (c) 2024, ZKA Web Services Co
 *
 */

#pragma once

#include <NewKit/Defines.hxx>

#define kHandoverMagic	 0xBADCC
#define kHandoverVersion 0x0117

/* Initial bitmap pointer location and size. */
#define kHandoverBitMapSz (gib_cast(4))

/* Executable base */
#define kHandoverExecBase (0x4000000)

#define kHandoverStructSz sizeof(HEL::HANDOVER_INFO_HEADER)

namespace Kernel::HEL
{
	/**
	@brief The executable type enum.
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
	@brief The executable architecture enum.
	*/

	enum
	{
		kArchAMD64 = 122,
		kArchARM64 = 123,
		kArchRISCV = 124,
		kArchCount = 3,
	};

	struct HANDOVER_INFO_HEADER final
	{
		UInt64 f_Magic;
		UInt64 f_Version;

		VoidPtr f_BitMapStart;
		SizeT	f_BitMapSize;

		VoidPtr f_PageStart;

		VoidPtr f_KernelImage;
		SizeT	f_KernelSz;
		VoidPtr f_StartupChime;
		SizeT	f_ChimeSz;
		VoidPtr f_StartupImage;
		SizeT	f_StartupSz;
		VoidPtr f_TTFallbackFont;
		SizeT	f_FontSz;

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

	/// @brief Alias of bootloader main type.
	typedef void (*HandoverProc)(HANDOVER_INFO_HEADER* handoverInfo);
} // namespace Kernel::HEL

/// @brief Bootloader global header.
inline Kernel::HEL::HANDOVER_INFO_HEADER* kHandoverHeader = nullptr;
