/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __ACPI_MANAGER__
#define __ACPI_MANAGER__

#include <Drivers/ACPI/ACPI.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace HCore {
class ACPIManager final {
 public:
  explicit ACPIManager(voidPtr rsdptr);
  ~ACPIManager() = default;

  ACPIManager &operator=(const ACPIManager &) = default;
  ACPIManager(const ACPIManager &) = default;

 public:
  void Shutdown();  // shutdown
  void Reset();     // soft-reboot

  ErrorOr<voidPtr> Find(const char *signature);

  bool Checksum(const char *checksum, SSizeT len);  // watch for collides!

 public:
  ErrorOr<voidPtr> operator[](const char *signature) {
    return this->Find(signature);
  }

 private:
  VoidPtr m_Rsdp;    // pointer to root descriptor.
  SSizeT m_Entries;  // number of entries, -1 tells that no invalid entries were
                     // found.
};
}  // namespace HCore

#endif  // !__ACPI_MANAGER__
