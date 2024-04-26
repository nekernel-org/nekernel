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

#include <FirmwareKit/EFI.hxx>
#include <BootKit/BootKit.hxx>
#include <BootKit/HW/ATA.hxx>

/// bugs: 0

#define kATADataLen 256

static Boolean kATADetected = false;
static Int32 kATADeviceType = kATADeviceCount;
static UInt16 kATAData[kATADataLen] = {0};

Boolean boot_ata_detected(Void);

STATIC Boolean boot_ata_wait_io(UInt16 IO) {
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

Void boot_ata_select(UInt16 Bus) {
  if (Bus == ATA_PRIMARY_IO)
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
  else
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean boot_ata_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus,
                      UInt8& OutMaster) {
  if (boot_ata_detected()) return true;

  BTextWriter writer;

  UInt16 IO = Bus;

  boot_ata_select(IO);

  // Bus init, NEIN bit.
  Out8(IO + ATA_REG_NEIN, 1);

  // identify until it's good.
ATAInit_Retry:
  auto statRdy = In8(IO + ATA_REG_STATUS);

  if (statRdy & ATA_SR_ERR) {
    writer.Write(
        L"New Boot: ATA: Select error, not an IDE based hard-drive.\r\n");

    return false;
  }

  if ((statRdy & ATA_SR_BSY)) goto ATAInit_Retry;

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  /// fetch serial info
  /// model, speed, number of sectors...

  boot_ata_wait_io(IO);

  for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData) {
    kATAData[indexData] = In8(IO + ATA_REG_DATA);
  }

  OutBus =
      (Bus == ATA_PRIMARY_IO) ? BootDeviceATA::kPrimary : BootDeviceATA::kSecondary;

  OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

  return true;
}

Void boot_ata_read(UInt64 Lba, UInt16 IO, UInt8 Master, CharacterTypeUTF8* Buf,
                   SizeT SectorSz, SizeT Size) {
  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  boot_ata_wait_io(IO);
  boot_ata_select(IO);

  Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));
  Out8(IO + ATA_REG_SEC_COUNT0, 1);

  Out8(IO + ATA_REG_LBA0, (Lba));
  Out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  Out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  Out8(IO + ATA_REG_LBA4, (Lba) >> 24);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff) {
    boot_ata_wait_io(IO);
    Buf[IndexOff] = In16(IO + ATA_REG_DATA);
  }
}

Void boot_ata_write(UInt64 Lba, UInt16 IO, UInt8 Master, CharacterTypeUTF8* Buf,
                    SizeT SectorSz, SizeT Size) {
  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  boot_ata_wait_io(IO);
  boot_ata_select(IO);

  Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));
  Out8(IO + ATA_REG_SEC_COUNT0, 1);

  Out8(IO + ATA_REG_LBA0, (Lba));
  Out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  Out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  Out8(IO + ATA_REG_LBA4, (Lba) >> 24);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff) {
    boot_ata_wait_io(IO);
    Out16(IO + ATA_REG_DATA, Buf[IndexOff]);
  }
}

/// @check is ATA detected?
Boolean boot_ata_detected(Void) { return kATADetected; }

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
  if (boot_ata_init(ATA_PRIMARY_IO, true, this->Leak().mBus,
                    this->Leak().mMaster) ||
      boot_ata_init(ATA_SECONDARY_IO, true, this->Leak().mBus,
                    this->Leak().mMaster)) {
    kATADetected = true;
  }
}
/**
 * @brief Is ATA detected?
 */
BootDeviceATA::operator bool() { return boot_ata_detected(); }

/**
    @brief Read Buf from disk
    @param Sz Sector size
    @param Buf buffer
*/
BootDeviceATA& BootDeviceATA::Read(CharacterTypeUTF8* Buf, const SizeT& SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  this->Leak().mErr = false;

  if (!Buf || SectorSz < 1) return *this;

  auto lba = this->Leak().mBase / BootDeviceATA::kSectorSize;

  boot_ata_read(lba, this->Leak().mBus, this->Leak().mMaster,
                Buf, SectorSz, this->Leak().mSize);

  return *this;
}

/**
    @brief Write Buf into disk
    @param Sz Sector size
    @param Buf buffer
*/
BootDeviceATA& BootDeviceATA::Write(CharacterTypeUTF8* Buf, const SizeT& SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  Leak().mErr = false;

  if (!Buf || SectorSz < 1) return *this;

  auto lba = this->Leak().mBase / BootDeviceATA::kSectorSize;

  boot_ata_write(lba, this->Leak().mBus, this->Leak().mMaster,
                Buf, SectorSz, this->Leak().mSize);

  return *this;
}

/**
 * @brief ATA trait getter.
 * @return BootDeviceATA::ATATrait& the drive config.
 */
BootDeviceATA::ATATrait& BootDeviceATA::Leak() { return mTrait; }

/***
    @brief Getter, gets the number of sectors inside the drive.
*/
SizeT BootDeviceATA::GetSectorsCount() noexcept {
    return kATAData[60] + kATAData[61];
}

SizeT BootDeviceATA::GetDiskSize() noexcept {
    Out8(this->Leak().mBus + ATA_REG_COMMAND, 0xF8);

    boot_ata_wait_io(this->Leak().mBus);

    UInt64 result = 0;

    result = In8(this->Leak().mBus + ATA_CYL_LOW);
    result += In8(this->Leak().mBus + ATA_CYL_MID) << 8;
    result += In8(this->Leak().mBus + ATA_CYL_HIGH) << 16;

    Out8(this->Leak().mBus + ATA_REG_CONTROL, 0x80);

    result += In8(this->Leak().mBus + ATA_CYL_LOW) << 24;
    result += In8(this->Leak().mBus + ATA_CYL_MID) << 32;
    result += In8(this->Leak().mBus + ATA_CYL_HIGH) << 40;

    BTextWriter writer;
    writer.Write(L"Device-Size: ").Write(result).Write(L"\r\n");

    return result;
}
