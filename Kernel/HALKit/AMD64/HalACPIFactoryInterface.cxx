/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <Builtins/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/String.hpp>
#include <ArchKit/ArchKit.hpp>
#include <KernelKit/KernelHeap.hpp>

namespace NewOS
{

	/// Custom to the virtual machine, you'll need to parse the MADT instead.

	void rt_shutdown_acpi_qemu_20(void)
	{
		HAL::Out16(0xb004, 0x2000);
	}

	void rt_shutdown_acpi_qemu_30_plus(void)
	{
		HAL::Out16(0x604, 0x2000);
	}

	void rt_shutdown_acpi_virtualbox(void)
	{
		HAL::Out16(0x4004, 0x3400);
	}

	/// You have to parse the MADT!

	ACPIFactoryInterface::ACPIFactoryInterface(voidPtr rsdPtr)
		: fRsdp(rsdPtr), fEntries(0)
	{
	}

	Void ACPIFactoryInterface::Shutdown()
	{
#ifdef __DEBUG__
		rt_shutdown_acpi_qemu_30_plus();
#else

#endif
	}

	/// @brief Reboot (shutdowns on qemu.)
	/// @return
	Void ACPIFactoryInterface::Reboot()
	{
#ifdef __DEBUG__
		rt_shutdown_acpi_qemu_30_plus();
#else

#endif
	}

	/// @brief Finds a descriptor table inside ACPI XSDT.
	ErrorOr<voidPtr> ACPIFactoryInterface::Find(const char* signature)
	{
		MUST_PASS(fRsdp);

		if (!signature)
			return ErrorOr<voidPtr>{-2};

		if (*signature == 0)
			return ErrorOr<voidPtr>{-3};

		RSDP* rsdPtr = reinterpret_cast<RSDP*>(this->fRsdp);

		if (rsdPtr->Revision <= 1)
		{
			return ErrorOr<voidPtr>{-4};
		}

		/// FIXME
		RSDT* xsdt = (RSDT*)rsdPtr->XsdtAddress;

		if (NewOS::HAL::ke_map_address((PDE*)hal_read_cr3(), rsdPtr->XsdtAddress, (UIntPtr)xsdt, NewOS::HAL::eFlagsRw))
			return ErrorOr<voidPtr>{-5};

		Int64 num = (xsdt->Length - sizeof(SDT)) / sizeof(UInt64);

		if (num < 1)
		{
			return ErrorOr<voidPtr>{-6};
		}

		this->fEntries = num;

		kcout << "ACPI: Number of entries: " << number(this->fEntries) << endl;
		kcout << "ACPI: Address of XSDT: " << hex_number((UIntPtr)xsdt) << endl;

		const short cAcpiSignatureLength = 4;

		for (Size index = 0; index < this->fEntries; ++index)
		{
			SDT* sdt = (SDT*)(xsdt->AddressArr[index]);

			for (short signature_index = 0; signature_index < cAcpiSignatureLength; ++signature_index)
			{
				if (sdt->Signature[signature_index] != signature[signature_index])
					break;

				if (signature_index == (cAcpiSignatureLength - 1))
					return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>(sdt));
			}
		}

		return ErrorOr<voidPtr>{nullptr};
	}

	/***
	@brief check SDT header
	@param checksum the header to checksum
	@param len the length of it.
*/
	bool ACPIFactoryInterface::Checksum(const char* checksum, SSizeT len)
	{
		if (len == 0)
			return -1;

		char chr = 0;

		for (int index = 0; index < len; ++index)
		{
			chr += checksum[index];
		}

		return chr == 0;
	}
} // namespace NewOS
