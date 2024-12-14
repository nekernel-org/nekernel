/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __ACPI__
#define __ACPI__

/**
	https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html
*/

#include <NewKit/Defines.h>

namespace Kernel
{
	class PACKED SDT
	{
	public:
		Char   Signature[4];
		UInt32 Length;
		UInt8  Revision;
		Char   Checksum;
		Char   OemId[6];
		Char   OemTableId[8];
		UInt32 OemRev;
		UInt32 CreatorID;
		UInt32 CreatorRevision;
	};

	class PACKED RSDP : public SDT
	{
	public:
		UInt32	RsdtAddress;
		UIntPtr XsdtAddress;
		UInt8	ExtendedChecksum;
		UInt8	Reserved0[3];
	};

	class PACKED ConfigHeader
	{
	public:
		UInt64 BaseAddress;
		UInt16 PciSegGroup;
		UInt8  StartBus;
		UInt8  EndBus;
		UInt32 Reserved;
	};

	enum ACPI_ADDRESS_SPACE_KIND : UInt8
	{
		eSystemMemory = 0,
		eSystemIO	  = 1,
		ePci		  = 2,
		eController	  = 3,
		eSmBus		  = 4,
		eCount		  = 5,
		eInvalid	  = 0xFF,
	};

	class PACKED ACPI_ADDRESS final
	{
	public:
		UInt8	AddressSpaceId;
		UInt8	RegisterBitWidth;
		UInt8	RegisterBitOffset;
		UInt8	Reserved;
		UIntPtr Address;
	};

	class PACKED RSDT final
	{
	public:
		Char   Signature[4];
		UInt32 Length;
		UInt8  Revision;
		Char   Checksum;
		Char   OemId[6];
		Char   OemTableId[8];
		UInt32 OemRev;
		UInt32 CreatorID;
		UInt32 CreatorRevision;
		UInt32 AddressArr[];
	};
} // namespace Kernel

#endif // !__ACPI__
