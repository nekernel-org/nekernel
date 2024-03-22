/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/**
 * @file ATA.cxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief ATA driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Mahrouss Logic
 *
 */

#include <Builtins/ATA/Defines.hxx>
#include <ArchKit/ArchKit.hpp>

#ifdef __KERNEL__

using namespace HCore;
using namespace HCore::HAL;

/// bugs: 0

#define kATADataLen 256

static Boolean kATADetected = false;
static Int32 kATADeviceType = kATADeviceCount;
static Char kATAData[kATADataLen] = {0};

Boolean drv_ata_wait_io(UInt16 IO) {
  for (int i = 0; i < 4; i++) In8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
  auto statRdy = In8(IO + ATA_REG_STATUS);

  if ((statRdy & ATA_SR_BSY)) goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
  statRdy = In8(IO + ATA_REG_STATUS);

  if (statRdy & ATA_SR_ERR) return false;

  if (!(statRdy & ATA_SR_DRDY)) goto ATAWaitForIO_Retry2;

  return true;
}

Void drv_ata_select(UInt16 Bus) {
  if (Bus == ATA_PRIMARY_IO)
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
  else
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean drv_ata_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus,
                      UInt8& OutMaster) {
  if (drv_ata_detected()) return false;

  UInt16 IO = Bus;

  drv_ata_select(IO);

  // Bus init, NEIN bit.
  Out8(IO + ATA_REG_NEIN, 1);

  // identify until it's good.
ATAInit_Retry:
  auto statRdy = In8(IO + ATA_REG_STATUS);

  if (statRdy & ATA_SR_ERR) {
    kcout << "NewKernel.exe: ATA: Select error, not an IDE based hard-drive.\r\n";

    return false;
  }

  if ((statRdy & ATA_SR_BSY)) goto ATAInit_Retry;

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  rt_set_memory(kATAData, 0, kATADataLen);

  /// fetch serial info
  /// model, speed, number of sectors...

  drv_ata_wait_io(IO);

  for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData) {
    kATAData[indexData] = In16(IO + ATA_REG_DATA);
  }

  OutBus = Bus;
  OutMaster = (OutBus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

  Out8(Bus + ATA_REG_HDDEVSEL, 0xA0 | ATA_MASTER << 4);

  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);

  unsigned cl = In8(Bus + ATA_CYL_LOW); /* get the "signature bytes" */
  unsigned ch = In8(Bus + ATA_CYL_HIGH);

  /* differentiate ATA, ATAPI, SATA and SATAPI */
  if (cl == 0x14 && ch == 0xEB) {
    kcout << "NewKernel.exe: PATAPI drive detected.\r\n";
    kATADeviceType = kATADevicePATA_PI;
  }
  if (cl == 0x69 && ch == 0x96) {
    kcout << "NewKernel.exe: SATAPI drive detected.\r\n";
    kATADeviceType = kATADeviceSATA_PI;
  }

  if (cl == 0x0 && ch == 0x0) {
    kcout << "NewKernel.exe: PATA drive detected.\r\n";
    kATADeviceType = kATADevicePATA;
  }

  if (cl == 0x3c && ch == 0xc3) {
    kcout << "NewKernel.exe: SATA drive detected.\r\n";
    kATADeviceType = kATADeviceSATA;
  }

  Out8(IO + ATA_REG_CONTROL, 0x02);

  return true;
}

Void drv_ata_read(UInt32 Lba, UInt16 IO, UInt8 Master, Char* Buf,
                   SizeT SectorSz, SizeT Size) {
  UInt8 Command = (!Master ? 0xE0 : 0xF0);

  Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0xF));
  Out8(IO + ATA_REG_SEC_COUNT0, SectorSz);

  Out8(IO + ATA_REG_LBA0, (UInt8)(Lba));
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba) >> 8);
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba) >> 16);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  drv_ata_wait_io(IO);

  for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff) {
    WideChar chr = In16(IO + ATA_REG_DATA);
    
    Buf[IndexOff] = chr;
  }
}

Void drv_ata_write(UInt32 Lba, UInt16 IO, UInt8 Master, Char* Buf,
                    SizeT SectorSz, SizeT Size) {
  UInt8 Command = (!Master ? 0xE0 : 0xF0);

  Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0xF));
  Out8(IO + ATA_REG_SEC_COUNT0, SectorSz);

  Out8(IO + ATA_REG_LBA0, (UInt8)(Lba));
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba) >> 8);
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba) >> 16);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  drv_ata_wait_io(IO);

  for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff) {
    Out16(IO + ATA_REG_DATA, Buf[IndexOff]);
    
  }
}

/// @check is ATA detected?
Boolean drv_ata_detected(Void) { return kATADetected; }

#endif // ifdef __KERNEL__
