/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Drivers/ACPI/ACPIManager.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/String.hpp>

namespace HCore {
ACPIManager::ACPIManager(voidPtr rsdPtr) : m_Rsdp(rsdPtr), m_Entries(0) {
  volatile RSDP *_rsdPtr = reinterpret_cast<volatile RSDP *>(this->m_Rsdp);

  MUST_PASS(_rsdPtr);
  MUST_PASS(_rsdPtr->Revision >= 2);
}

void ACPIManager::Shutdown() {}
void ACPIManager::Reset() {}

/// @brief Finds a descriptor table inside ACPI XSDT.
ErrorOr<voidPtr> ACPIManager::Find(const char *signature) {
  MUST_PASS(m_Rsdp);

  if (!signature) return ErrorOr<voidPtr>{-2};

  if (*signature == 0) return ErrorOr<voidPtr>{-3};

  RSDP *rsdPtr = reinterpret_cast<RSDP *>(this->m_Rsdp);

  auto xsdt = rsdPtr->XsdtAddress;
  SizeT num = (rsdPtr->Length + sizeof(SDT)) / 8;

  for (Size index = 0; index < num; ++index) {
    SDT *sdt = reinterpret_cast<SDT *>(xsdt + sizeof(SDT) + index * 8);

    if (!Checksum(sdt->Signature, 4)) ke_stop(RUNTIME_CHECK_ACPI);

    if (StringBuilder::Equals(const_cast<const char *>(sdt->Signature),
                              signature))
      return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>(sdt));
  }

  return ErrorOr<voidPtr>{-1};
}

/***
    @brief check SDT header
    @param checksum the header to checksum
    @param len the length of it.
*/
bool ACPIManager::Checksum(const char *checksum, SSizeT len) {
  if (len == 0) return -1;

  char chr = 0;

  for (int index = 0; index < len; ++index) {
    chr += checksum[index];
  }

  return chr == 0;
}

/// Custom to the virtual machine, you'll need to parse the MADT instead.

void rt_shutdown_acpi_qemu_20(void) { HAL::Out16(0xb004, 0x2000); }

void rt_shutdown_acpi_qemu_30_plus(void) { HAL::Out16(0x604, 0x2000); }

void rt_shutdown_acpi_virtualbox(void) { HAL::Out16(0x4004, 0x3400); }

/// You have to parse the MADT!
}  // namespace HCore
