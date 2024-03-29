/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Builtins/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/String.hpp>

namespace NewOS {

/// Custom to the virtual machine, you'll need to parse the MADT instead.

void rt_shutdown_acpi_qemu_20(void) { HAL::Out16(0xb004, 0x2000); }

void rt_shutdown_acpi_qemu_30_plus(void) { HAL::Out16(0x604, 0x2000); }

void rt_shutdown_acpi_virtualbox(void) { HAL::Out16(0x4004, 0x3400); }

/// You have to parse the MADT!

ACPIFactoryInterface::ACPIFactoryInterface(voidPtr rsdPtr)
    : m_Rsdp(rsdPtr), m_Entries(0) {
  volatile RSDP *_rsdPtr = reinterpret_cast<volatile RSDP *>(this->m_Rsdp);

  MUST_PASS(_rsdPtr);
  MUST_PASS(_rsdPtr->Revision >= 2);
}

Void ACPIFactoryInterface::Shutdown() {
#ifdef __DEBUG__
  rt_shutdown_acpi_qemu_30_plus();
#else

#endif
}

/// @brief Reboot (shutdowns on qemu.)
/// @return 
Void ACPIFactoryInterface::Reboot() {
#ifdef __DEBUG__
  rt_shutdown_acpi_qemu_30_plus();
#else

#endif
}

/// @brief Finds a descriptor table inside ACPI XSDT.
ErrorOr<voidPtr> ACPIFactoryInterface::Find(const char *signature) {
  MUST_PASS(m_Rsdp);

  if (!signature) return ErrorOr<voidPtr>{-2};

  if (*signature == 0) return ErrorOr<voidPtr>{-3};

  RSDP *rsdPtr = reinterpret_cast<RSDP *>(this->m_Rsdp);

  if (rsdPtr->Revision <= 1) {
    return ErrorOr<voidPtr>{-4};
  }

  SDT* xsdt = (SDT*)(rsdPtr->XsdtAddress >> (rsdPtr->XsdtAddress & 0xfff));
  SizeT num = (xsdt->Length + sizeof(SDT)) / 8;

  kcout << "ACPI: Number of entries: " << number(num) << endl;

  constexpr short ACPI_SIGNATURE_LENGTH = 4;

  for (Size index = 0; index < num; ++index) {
    SDT *sdt = (SDT*)*((UInt64*)(UInt64)xsdt + sizeof(SDT) + (index * 8));

		for (int signature_index = 0; signature_index < 4; signature_index++){
			if (sdt->Signature[signature_index] != signature[signature_index])
				break;

			if (signature_index == 3) return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>((SDT*)sdt));;
		}
  }

  return ErrorOr<voidPtr>{-1};
}

/***
    @brief check SDT header
    @param checksum the header to checksum
    @param len the length of it.
*/
bool ACPIFactoryInterface::Checksum(const char *checksum, SSizeT len) {
  if (len == 0) return -1;

  char chr = 0;

  for (int index = 0; index < len; ++index) {
    chr += checksum[index];
  }

  return chr == 0;
}
}  // namespace NewOS
