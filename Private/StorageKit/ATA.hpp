/*
*	========================================================
*
*	HCore
* 	Copyright Mahrouss Logic, all rights reserved.
*
* 	========================================================
*/

#pragma once

#include <KernelKit/PCI/Dma.hpp>
#include <KernelKit/PCI/IO.hpp>
#include <NewKit/Defines.hpp>
#include <StorageKit/PRDT.hpp>
#include <KernelKit/DebugOutput.hpp>

namespace HCore
{
    enum class PATAType
    {
        kRead28,
        kRead48,
        kWrite28,
        kWrite48,
        kATAUnknown
    };

    const char* ata_read_28(ULong lba);
    const char* ata_read_48(ULong lba);

    Int32 ata_write_48(ULong lba, const char *text = nullptr);
    Int32 ata_write_28(ULong lba, const char *text = nullptr);

    class PATACommandManager final
    {
    public:
        PATACommandManager() = default;
        ~PATACommandManager() = default;

        PATACommandManager &operator=(const PATACommandManager &) = default;
        PATACommandManager(const PATACommandManager &) = default;

    public:
        static Ref<PATACommandManager> Shared()
        {
           static Ref<PATACommandManager> manager;
           return manager;
        }

    public:
        template <PATAType Command>
        ErrorOr<const char*> operator()(ULong lba, const char *text = nullptr) noexcept
        {
           switch (Command)
           {
           case PATAType::kRead28:
               return ErrorOr<const char*>(ata_read_28(lba));
           case PATAType::kRead48:
               return ErrorOr<const char*>(ata_read_48(lba));
           case PATAType::kWrite28:
           {
               if (text)
               {
                   ata_write_28(lba, text);
                   kcout << "ErrorOr<CT> ata_read<CT, Command>: Write ATA Command... "
                            "(Write28)\n";

                   return {};
               }

               kcout << "ErrorOr<CT> ata_read<CT, Command>: Bad ATA Command... "
                        "(Write28)\n";

               return {};
           }
           case PATAType::kWrite48:
           {
               if (text)
               {
                   ata_write_48(lba, text);
                   kcout << "ErrorOr<CT> ata_read<CT, Command>: Write ATA Command... "
                            "(Write48)\n";

                   return {};
               }

               kcout << "ErrorOr<CT> ata_read<CT, Command>: Bad ATA Command... "
                        "(Write48)\n";

               return {};
           }
           case PATAType::kATAUnknown:
           {
               kcout << "ErrorOr<CT> ata_read<CT, Command>: Unknown ATA Command...\n";
               return {};
           }
           }

           return ErrorOr<const char*>(nullptr);
        }

    };
} // namespace HCore
