/*
 * Copyright Mahrouss Logic, all rights reserved
 */

#include "ATA.hxx"
#include <ArchKit/Arch.hpp>

using namespace hCore::HAL;

static Boolean kATADetected = false;

void IDESelect(UInt8 Bus, Boolean isMaster)
{
    if (Bus == ATA_PRIMARY)
        out8(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
             isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
    else
        out8(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL,
             isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
}

Boolean ATAInitDriver(UInt8 Bus, UInt8 Drive)
{
    UInt16 IO = (Bus == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    IDESelect(Bus, Drive);

    out8(IO + ATA_REG_SEC_COUNT0, 0);
    out8(IO + ATA_REG_LBA0, 0);
    out8(IO + ATA_REG_LBA1, 0);
    out8(IO + ATA_REG_LBA2, 0);

    out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    UInt8 status = in8(IO + ATA_REG_STATUS);

    if (status)
    {
        while ((status = in8(IO + ATA_REG_STATUS) & ATA_SR_BSY))
            ;

        if (status & ATA_REG_ERROR)
        {
            return false;
        }

        kATADetected = true;
        return status;
    }

    return false;
}

void ATAWait(UInt16 IO)
{
    for (int i = 0; i < 4000; i++)
        in8(IO + ATA_REG_ALT_STATUS);
}

void IDEPoll(UInt16 IO) { ATAWait(IO); }

UInt16 ATAReadLba(UInt32 lba, UInt8 bus, Boolean master)
{
    UInt16 IO = bus;
    IDESelect(IO, master ? ATA_MASTER : ATA_SLAVE);

    out8(IO + ATA_REG_LBA5, (UInt8)(lba >> 24) & 0xF);

    out8(IO + ATA_REG_SEC_COUNT0, lba / 512);

    out8(IO + ATA_REG_LBA0, (UInt8)lba);
    out8(IO + ATA_REG_LBA1, (UInt8)(lba >> 8));
    out8(IO + ATA_REG_LBA2, (UInt8)(lba >> 16));

    out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    IDEPoll(IO);

    UInt16 data = in16(IO + ATA_REG_DATA);

    while ((in8(ATA_COMMAND(IO))) & 0x88)
        ATAWait(IO);

    return data;
}

Void ATAWriteLba(UInt16 Byte, UInt32 lba, UInt8 bus, Boolean master)
{
    UInt16 IO = bus;
    IDESelect(IO, master ? ATA_MASTER : ATA_SLAVE);

    out8(IO + ATA_REG_LBA5, (UInt8)(lba >> 24) & 0xF);

    out8(IO + ATA_REG_SEC_COUNT0, lba / 512);

    out8(IO + ATA_REG_LBA0, (UInt8)lba);
    out8(IO + ATA_REG_LBA1, (UInt8)(lba >> 8));
    out8(IO + ATA_REG_LBA2, (UInt8)(lba >> 16));

    out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO); // TODO: support DMA

    IDEPoll(IO);

    out32(IO + ATA_REG_DATA, Byte);

    while ((in8(ATA_COMMAND(IO))) & 0x88)
        ATAWait(IO);
}

Boolean ATAIsDetected(Void) { return kATADetected; }

extern "C" void _start(void)
{
    ATAInitDriver(ATA_PRIMARY, true);
    ATAInitDriver(ATA_PRIMARY, false);

    ATAInitDriver(ATA_SECONDARY, true);
    ATAInitDriver(ATA_SECONDARY, false);
}

extern "C" void out8(UInt16 port, UInt8 value)
{
    asm volatile("outb %%al, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" void out16(UInt16 port, UInt16 value)
{
    asm volatile("outw %%ax, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" void out32(UInt16 port, UInt32 value)
{
    asm volatile("outl %%eax, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" UInt8 in8(UInt16 port)
{
    UInt8 value = 0UL;
    asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

    return value;
}

extern "C" UInt16 in16(UInt16 port)
{
    UInt16 value = 0UL;
    asm volatile("inw %1, %%ax" : "=a"(value) : "Nd"(port) : "memory");

    return value;
}

extern "C" UInt32 in32(UInt16 port)
{
    UInt32 value = 0UL;
    asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

    return value;
}

extern "C" int init_ata_mpt(void)
{
    for (int i = 0; i < 255; ++i)
    {
        if (ATAInitDriver(i, ATA_MASTER))
        {
            ATAInitDriver(i, ATA_SLAVE);
            return 1;
        }
    }

    return 0;
}