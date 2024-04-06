/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Defines.hxx
    Purpose: ATA header.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

///! Status register
#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01

///! Error register
#define ATA_ER_BBK 0x80
#define ATA_ER_UNC 0x40
#define ATA_ER_MC 0x20
#define ATA_ER_IDNF 0x10
#define ATA_ER_MCR 0x08
#define ATA_ER_ABRT 0x04
#define ATA_ER_TK0NF 0x02
#define ATA_ER_AMNF 0x01

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

///! ident offsets, use with data that we got from ATA_CMD_IDENTIFY.
#define ATA_IDENT_DEVICE_TYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

///! Register
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SEC_COUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SEC_COUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALT_STATUS 0x0C
#define ATA_REG_DEV_ADDRESS 0x0D

#define ATA_REG_NEIN 0x01

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

///! Irq
#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

///! Channels
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_CYL_LOW 4
#define ATA_CYL_HIGH 5

///! IO Direction
#define ATA_READ 0x00
#define ATA_WRITE 0x013

#define ATA_PRIMARY_SEL 0xA0
#define ATA_SECONDARY_SEL 0xB0

///! ATA address register.
#define ATA_ADDRESS1(x) (x + 3)
#define ATA_ADDRESS2(x) (x + 4)
#define ATA_ADDRESS3(x) (x + 5)

///! ATA command register.
#define ATA_COMMAND(x)  (x + 7)

#define kATASectorSize (512U)

enum {
  kATADevicePATA,
  kATADeviceSATA,
  kATADevicePATA_PI,
  kATADeviceSATA_PI,
  kATADeviceCount,
};

#if defined(__ATA_PIO__) || defined(__ATA_DMA__)

NewOS::Boolean drv_std_init(NewOS::UInt16 Bus, NewOS::UInt8 Drive, NewOS::UInt16& OutBus,
                      NewOS::UInt8& OutMaster);

NewOS::Boolean drv_std_detected(NewOS::Void);

NewOS::Void drv_std_select(NewOS::UInt16 Bus);

NewOS::Boolean drv_std_wait_io(NewOS::UInt16 IO);

NewOS::Void drv_std_read(NewOS::UInt64 Lba, NewOS::UInt16 IO, NewOS::UInt8 Master, NewOS::Char* Buf,
                   NewOS::SizeT SectorSz, NewOS::SizeT Size);

NewOS::Void drv_std_write(NewOS::UInt64 Lba, NewOS::UInt16 IO, NewOS::UInt8 Master, NewOS::Char* Buf,
                    NewOS::SizeT SectorSz, NewOS::SizeT Size);

#endif // ifdef __KERNEL__
