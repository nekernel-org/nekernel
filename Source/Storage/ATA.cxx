/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <StorageKit/ATA.hpp>
#include <ArchKit/Arch.hpp>

//! @brief Driver for ATA, listens to a specific address for data to come.
//! mapped by NewFirmware.

#define kATAError 2

namespace hCore
{
    Ref<PRDT*> kPrdt = nullptr;

    bool set_prdt_struct(Ref<PRDT*>& refCtrl)
    {
        if (!kPrdt)
        {
            kPrdt = refCtrl;
            kcout << "[set_prdt_struct] PRDT is set.";

            return true;
        }

        kcout << "[set_prdt_struct] [WARNING] Tried to change PRDT.\n";
        return false;
    }

    enum
    {
        k28BitRead = 0xC8,
        k48BitRead = 0x25,
        k28BitWrite = 0xCA,
        k48BitWrite = 0x35,
    };

    const char* ata_read_28(ULong lba)
    {
        static char buffer[512];

        UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

        packet[0] = k28BitRead;
        packet[1] = (UIntPtr)&buffer;
        packet[2] = lba;

        rt_wait_for_io();

        return buffer;
    }

    const char* ata_read_48(ULong lba)
    {
        static char buffer[512];

        UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

        packet[0] = k48BitRead;
        packet[1] = (UIntPtr)&buffer;
        packet[4] = lba;

        rt_wait_for_io();

        return buffer;
    }

    Int32 ata_write_48(ULong lba, const char *text)
    {
         UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

        packet[0] = k48BitWrite;
        packet[1] = (UIntPtr)&text;
        packet[2] = lba;

        rt_wait_for_io();

        return packet[1] == 2 ? kATAError : 0;
    }

    Int32 ata_write_28(ULong lba, const char *text)
    {
        UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

        packet[0] = k28BitWrite;
        packet[1] = (UIntPtr)&text;
		packet[2] = lba;

        rt_wait_for_io();

        return packet[1] == 2 ? kATAError : 0;
    }
} // namespace hCore
