/* -------------------------------------------

	Copyright ZKA Technologies

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

#include <NewKit/Defines.hpp>

/* useful macros */

#define kHandoverMagic	 0xBADCC
#define kHandoverVersion 0x112

#define kHandoverStructSz sizeof(HEL::HandoverHeader)

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
		kTypeCount		  = 3,
	};

	/**
	@brief The executable architecture.
*/

	enum
	{
		kArchAmd64 = 122,
		kArchCount = 2,
	};

	/**
@brief The first struct that we read when inspecting The executable
it tells us more about it and IS format independent.
*/
	typedef struct HandoverHeader final
	{
		UInt64	f_TargetMagic;
		Int32	f_TargetType;
		Int32	f_TargetArch;
		UIntPtr f_TargetStartAddress;
	} __attribute__((packed)) HandoverHeader, *HandoverHeaderPtr;

	struct HandoverInformationHeader
	{
		UInt64 f_Magic;
		UInt64 f_Version;

		voidPtr f_VirtualStart;
		SizeT	f_VirtualSize;
		voidPtr f_PhysicalStart;

		WideChar f_FirmwareVendorName[32];
		SizeT	 f_FirmwareVendorLen;

		struct
		{
			VoidPtr f_SmBios;
			VoidPtr f_RsdPtr;
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
