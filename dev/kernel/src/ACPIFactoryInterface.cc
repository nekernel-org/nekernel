/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/HeapMgr.h>
#include <NeKit/KString.h>
#include <modules/ACPI/ACPIFactoryInterface.h>

namespace Kernel {
constexpr STATIC const auto kMinACPIVer = 1U;

/// @brief Finds a descriptor table inside ACPI XSDT.
ErrorOr<voidPtr> ACPIFactoryInterface::Find(const Char* signature) {
  if (this->fRsdp) return ErrorOr<voidPtr>{kErrorInvalidData};
  if (!signature) return ErrorOr<voidPtr>{-kErrorInvalidData};
  if (*signature == 0) return ErrorOr<voidPtr>{-kErrorInvalidData};

  RSDP* rsp_ptr = reinterpret_cast<RSDP*>(this->fRsdp);

  if (rsp_ptr->Revision < kMinACPIVer) return ErrorOr<voidPtr>{-kErrorInvalidData};

  RSDT* xsdt = reinterpret_cast<RSDT*>(rsp_ptr->RsdtAddress);

  Int64 num = (xsdt->Length - sizeof(SDT)) / sizeof(Int64);

  /***
    crucial to avoid underflows.
  */
  if (num < 1) {
    /// stop here, we should have entries...
    ke_panic(RUNTIME_CHECK_ACPI);
    return ErrorOr<voidPtr>{-kErrorInvalidData};
  }

  this->fEntries = num;

  (Void)(kout << "ACPI: Number of entries: " << number(this->fEntries) << kendl);
  (Void)(kout << "ACPI: Revision: " << number(xsdt->Revision) << kendl);
  (Void)(kout << "ACPI: Signature: " << xsdt->Signature << kendl);
  (Void)(kout << "ACPI: Address of XSDT: " << hex_number((UIntPtr) xsdt) << kendl);

  const short cAcpiSignatureLength = 4;

  for (Size index = 0; index < this->fEntries; ++index) {
    SDT* sdt = reinterpret_cast<SDT*>(xsdt->AddressArr[index]);

    (Void)(kout << "ACPI: Checksum: " << number(sdt->Checksum) << kendl);
    (Void)(kout << "ACPI: Revision: " << number(sdt->Revision) << kendl);

    for (short signature_index = 0; signature_index < cAcpiSignatureLength; ++signature_index) {
      if (sdt->Signature[signature_index] != signature[signature_index]) break;

      if (signature_index == (cAcpiSignatureLength - 1)) {
        (Void)(kout << "ACPI: SDT Signature: " << sdt->Signature << kendl);
        (Void)(kout << "ACPI: SDT OEM ID: " << sdt->OemId << kendl);
        return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>(xsdt->AddressArr[index]));
      }
    }
  }

  return ErrorOr<voidPtr>{-kErrorInvalidData};
}

/***
@brief Checksum on SDT header.
@param checksum the header to checksum
@param len the length of it.
*/
bool ACPIFactoryInterface::Checksum(const Char* checksum, SSizeT len) {
  if (len == 0) return 1;

  char chr = 0;

  for (int index = 0; index < len; ++index) {
    chr += checksum[index];
  }

  return chr == 0;
}

ErrorOr<voidPtr> ACPIFactoryInterface::operator[](const Char* signature) {
  return this->Find(signature);
}
}  // namespace Kernel
