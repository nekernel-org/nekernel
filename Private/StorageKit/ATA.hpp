/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/PCI/Dma.hpp>
#include <KernelKit/PCI/IO.hpp>
#include <NewKit/Defines.hpp>
#include <StorageKit/PRDT.hpp>

namespace HCore {
enum class PATAType { kRead28, kRead48, kWrite28, kWrite48, kATAUnknown };

const char *ata_read_28(ULong lba);
const char *ata_read_48(ULong lba);

Int32 ata_write_48(ULong lba, char *text = nullptr);
Int32 ata_write_28(ULong lba, char *text = nullptr);

class PATACommandFactory final {
 public:
  explicit PATACommandFactory() = default;
  ~PATACommandFactory() = default;

  PATACommandFactory &operator=(const PATACommandFactory &) = default;
  PATACommandFactory(const PATACommandFactory &) = default;

 public:
  static Ref<PATACommandFactory> Shared() {
    static Ref<PATACommandFactory> manager;
    return manager;
  }

 public:
  template <PATAType Command>
  ErrorOr<const char *> operator()(ULong lba,
                                   const char *text = nullptr) noexcept {
    switch (Command) {
      case PATAType::kRead28:
        return ErrorOr<const char *>(ata_read_28(lba));
      case PATAType::kRead48:
        return ErrorOr<const char *>(ata_read_48(lba));
      case PATAType::kWrite28: {
        if (text) {
          ata_write_28(lba, text);
          kcout << "ErrorOr<CT> ata_read<CT, Command>: Write ATA Command... "
                   "(Write28)\n";

          return {};
        }

        kcout << "ErrorOr<CT> ata_read<CT, Command>: Bad ATA Command... "
                 "(Write28)\n";

        return {};
      }
      case PATAType::kWrite48: {
        if (text) {
          ata_write_48(lba, text);
          kcout << "ErrorOr<CT> ata_read<CT, Command>: Write ATA Command... "
                   "(Write48)\n";

          return {};
        }

        kcout << "ErrorOr<CT> ata_read<CT, Command>: Bad ATA Command... "
                 "(Write48)\n";

        return {};
      }
      case PATAType::kATAUnknown: {
        kcout << "ErrorOr<CT> ata_read<CT, Command>: Unknown ATA Command...\n";
        return {};
      }
    }

    return ErrorOr<const char *>(nullptr);
  }
};
}  // namespace HCore
