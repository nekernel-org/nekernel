/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss/Symphony Corp, all rights reserved.

------------------------------------------- */

/**
 * @file BootATA.cc
 * @author Amlal El Mahrouss (amlal@nekernel.org)
 * @brief ATA driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal El Mahrouss
 *
 */

#include <BootKit/BootKit.h>
#include <BootKit/HW/ATA.h>
#include <FirmwareKit/EFI.h>

#define kATADataLen (256)

/// bugs: 0

using namespace Boot;

static Boolean kATADetected          = false;
static UInt16  kATAData[kATADataLen] = {0};

Boolean boot_ata_detected(Void);

STATIC Boolean boot_ata_wait_io(UInt16 IO) {
  for (int i = 0; i < 400; i++) rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
  auto status_rdy = rt_in8(IO + ATA_REG_STATUS);

  if ((status_rdy & ATA_SR_BSY)) goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
  status_rdy = rt_in8(IO + ATA_REG_STATUS);

  if (status_rdy & ATA_SR_ERR) return false;

  if (!(status_rdy & ATA_SR_DRDY)) goto ATAWaitForIO_Retry2;

  return true;
}

Void boot_ata_select(UInt16 Bus) {
  if (Bus == ATA_PRIMARY_IO)
    rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
  else
    rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean boot_ata_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus, UInt8& OutMaster) {
  NE_UNUSED(Drive);

  if (boot_ata_detected()) return true;

  BootTextWriter writer;

  UInt16 IO = Bus;

  boot_ata_select(IO);

  // Bus init, NEIN bit.
  rt_out8(IO + ATA_REG_NEIN, 1);

  // identify until it's good.
ATAInit_Retry:
  auto status_rdy = rt_in8(IO + ATA_REG_STATUS);

  if (status_rdy & ATA_SR_ERR) {
    writer.Write(L"VMBoot: ATA: Not an IDE based drive.\r");

    return false;
  }

  if ((status_rdy & ATA_SR_BSY)) goto ATAInit_Retry;

  boot_ata_select(IO);

  rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  /// fetch serial info
  /// model, speed, number of sectors...

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ))
    ;

  for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData) {
    kATAData[indexData] = rt_in16(IO + ATA_REG_DATA);
  }

  OutBus = (Bus == ATA_PRIMARY_IO) ? BootDeviceATA::kPrimary : BootDeviceATA::kSecondary;

  OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

  return true;
}

Void boot_ata_read(UInt64 Lba, UInt16 IO, UInt8 Master, CharacterTypeASCII* Buf, SizeT SectorSz,
                   SizeT Size) {
  Lba /= SectorSz;

  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  boot_ata_wait_io(IO);
  boot_ata_select(IO);

  rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

  rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz) / SectorSz));

  rt_out8(IO + ATA_REG_LBA0, (Lba) &0xFF);
  rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

  rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ))
    ;

  for (SizeT IndexOff = 0; IndexOff < Size; IndexOff += 2) {
    boot_ata_wait_io(IO);

    auto in = rt_in16(IO + ATA_REG_DATA);

    Buf[IndexOff]     = in & 0xFF;
    Buf[IndexOff + 1] = (in >> 8) & 0xFF;
    boot_ata_wait_io(IO);
  }
}

Void boot_ata_write(UInt64 Lba, UInt16 IO, UInt8 Master, CharacterTypeASCII* Buf, SizeT SectorSz,
                    SizeT Size) {
  Lba /= SectorSz;

  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  boot_ata_wait_io(IO);
  boot_ata_select(IO);

  rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

  rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz)) / SectorSz));

  rt_out8(IO + ATA_REG_LBA0, (Lba) &0xFF);
  rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

  rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ))
    ;

  for (SizeT IndexOff = 0; IndexOff < Size; IndexOff += 2) {
    boot_ata_wait_io(IO);

    UInt8  low    = (UInt8) Buf[IndexOff];
    UInt8  high   = (IndexOff + 1 < Size) ? (UInt8) Buf[IndexOff + 1] : 0;
    UInt16 packed = (high << 8) | low;

    rt_out16(IO + ATA_REG_DATA, packed);

    boot_ata_wait_io(IO);
  }

  boot_ata_wait_io(IO);
}

/// @check is ATA detected?
Boolean boot_ata_detected(Void) {
  return kATADetected;
}

/***
 *
 *
 * @brief ATA Device class.
 *
 *
 */

/**
 * @brief ATA Device constructor.
 * @param void none.
 */
BootDeviceATA::BootDeviceATA() noexcept {
  if (boot_ata_init(ATA_PRIMARY_IO, true, this->Leak().mBus, this->Leak().mMaster) ||
      boot_ata_init(ATA_SECONDARY_IO, true, this->Leak().mBus, this->Leak().mMaster)) {
    kATADetected = true;
  }
}
/**
 * @brief Is ATA detected?
 */
BootDeviceATA::operator bool() {
  return boot_ata_detected();
}

/**
  @brief Read Buf from disk
  @param Sz Sector size
  @param Buf buffer
*/
BootDeviceATA& BootDeviceATA::Read(CharacterTypeASCII* Buf, SizeT SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  this->Leak().mErr = false;

  if (!Buf || SectorSz < 1) return *this;

  boot_ata_read(this->Leak().mBase, this->Leak().mBus, this->Leak().mMaster, Buf, SectorSz,
                this->Leak().mSize);

  return *this;
}

/**
  @brief Write Buf into disk
  @param Sz Sector size
  @param Buf buffer
*/
BootDeviceATA& BootDeviceATA::Write(CharacterTypeASCII* Buf, SizeT SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  Leak().mErr = false;

  if (!Buf || SectorSz < 1 || this->Leak().mSize < 1) {
    Leak().mErr = true;
    return *this;
  }

  boot_ata_write(this->Leak().mBase, this->Leak().mBus, this->Leak().mMaster, Buf, SectorSz,
                 this->Leak().mSize);

  return *this;
}

/**
 * @brief ATA trait getter.
 * @return BootDeviceATA::ATATrait& the drive config.
 */
BootDeviceATA::ATATrait& BootDeviceATA::Leak() {
  return mTrait;
}

/***
  @brief Getter, gets the number of sectors inside the drive.
*/
SizeT BootDeviceATA::GetSectorsCount() noexcept {
  return (kATAData[61] << 16) | kATAData[60];
}

SizeT BootDeviceATA::GetDiskSize() noexcept {
  return this->GetSectorsCount() * BootDeviceATA::kSectorSize;
}
