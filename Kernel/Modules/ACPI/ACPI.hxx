/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#ifndef __ACPI__
#define __ACPI__

/**
	https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html
*/

#include <NewKit/Defines.hpp>

namespace NewOS
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

	enum class AddressSpace : UInt8
	{
		SystemMemory = 0,
		SystemIO	 = 1,
		Pci			 = 2,
		Controller	 = 3,
		SmBus		 = 4,
		Count		 = 5,
		Invalid		 = 0xFF,
	};

	class PACKED Address
	{
	public:
		AddressSpace AddressSpaceId;
		UInt8		 RegisterBitWidth;
		UInt8		 RegisterBitOffset;
		UInt8		 Reserved;
		UIntPtr		 Address;
	};

	class PACKED RSDT
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
} // namespace NewOS

#endif // !__ACPI__
