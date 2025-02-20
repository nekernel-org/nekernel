/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <NewKit/KString.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/MemoryMgr.h>

namespace NeOS
{
	/// @brief Finds a descriptor table inside ACPI XSDT.
	ErrorOr<voidPtr> ACPIFactoryInterface::Find(const Char* signature)
	{
		MUST_PASS(this->fRsdp);

		if (!signature)
			return ErrorOr<voidPtr>{-1};

		if (*signature == 0)
			return ErrorOr<voidPtr>{-1};

		RSDP* rsp_ptr = reinterpret_cast<RSDP*>(this->fRsdp);

		if (rsp_ptr->Revision <= 1)
			return ErrorOr<voidPtr>{-1};

		RSDT* xsdt = reinterpret_cast<RSDT*>(rsp_ptr->RsdtAddress);

		Int64 num = (xsdt->Length - sizeof(SDT)) / sizeof(Int64);

		/***
			crucial to avoid underflows.
		*/
		if (num < 1)
		{
			/// stop here, we should have entries...
			ke_panic(RUNTIME_CHECK_ACPI);
			return ErrorOr<voidPtr>{-1};
		}

		this->fEntries = num;

		kout << "ACPI: Number of entries: " << number(this->fEntries) << endl;
		kout << "ACPI: Revision: " << number(xsdt->Revision) << endl;
		kout << "ACPI: Signature: " << xsdt->Signature << endl;
		kout << "ACPI: Address of XSDT: " << hex_number((UIntPtr)xsdt) << endl;

		const short cAcpiSignatureLength = 4;

		for (Size index = 0; index < this->fEntries; ++index)
		{
			SDT* sdt = reinterpret_cast<SDT*>(xsdt->AddressArr[index]);

			kout << "ACPI: Checksum: " << number(sdt->Checksum) << endl;
			kout << "ACPI: Revision: " << number(sdt->Revision) << endl;

			for (short signature_index = 0; signature_index < cAcpiSignatureLength; ++signature_index)
			{
				if (sdt->Signature[signature_index] != signature[signature_index])
					break;

				if (signature_index == (cAcpiSignatureLength - 1))
				{
					kout << "ACPI: SDT Signature: " << sdt->Signature << endl;
					kout << "ACPI: SDT OEM ID: " << sdt->OemId << endl;
					return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>(xsdt->AddressArr[index]));
				}
			}
		}

		return ErrorOr<voidPtr>{-1};
	}

	/***
	@brief Checksum on SDT header.
	@param checksum the header to checksum
	@param len the length of it.
	*/
	bool ACPIFactoryInterface::Checksum(const Char* checksum, SSizeT len)
	{
		if (len == 0)
			return 1;

		char chr = 0;

		for (int index = 0; index < len; ++index)
		{
			chr += checksum[index];
		}

		return chr == 0;
	}
} // namespace NeOS
