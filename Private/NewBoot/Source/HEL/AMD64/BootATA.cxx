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

#include <BootKit/Arch/ATA.hxx>
#include <BootKit/BootKit.hxx>

/// bugs: 0

#define kATADataLen 256

static Boolean kATADetected = false;
static Int32 kATADeviceType = kATADeviceCount;
static CharacterType kATAData[kATADataLen] = {0};

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

Void ATASelect(UInt16 Bus) {
  if (Bus == ATA_PRIMARY_IO)
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
  else
    Out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean boot_ata_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus,
                      UInt8& OutMaster) {
  if (boot_ata_detected()) return false;

  BTextWriter writer;

  UInt16 IO = Bus;

  ATASelect(IO);

  // Bus init, NEIN bit.
  Out8(IO + ATA_REG_NEIN, 1);

  // identify until it's good.
ATAInit_Retry:
  auto statRdy = In8(IO + ATA_REG_STATUS);

  if (statRdy & ATA_SR_ERR) {
    writer.Write(
        L"NewBoot: ATA: Select error, not an IDE based hard-drive.\r\n");

    return false;
  }

  if ((statRdy & ATA_SR_BSY)) goto ATAInit_Retry;

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  BSetMem(kATAData, 0, kATADataLen);

  /// fetch serial info
  /// model, speed, number of sectors...

  for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData) {
    kATAData[indexData] = In16(IO + ATA_REG_DATA);
  }

  writer.Write(L"NewBoot: Model: ");

  for (SizeT indexData = 0; indexData < kATADataLen; indexData += 1) {
    writer.WriteCharacter(kATAData[indexData + ATA_IDENT_MODEL + 1])
        .WriteCharacter(kATAData[indexData + ATA_IDENT_MODEL]);
  }

  writer.Write(L"\r\n");

  OutBus = (Bus == ATA_PRIMARY) ? BDeviceATA::kPrimary : BDeviceATA::kSecondary;
  OutMaster = (Bus == ATA_PRIMARY) ? ATA_MASTER : ATA_SLAVE;

  Out8(Bus + ATA_REG_HDDEVSEL, 0xA0 | ATA_MASTER << 4);

  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);
  In8(Bus + ATA_REG_CONTROL);

  unsigned cl = In8(Bus + ATA_CYL_LOW); /* get the "signature bytes" */
  unsigned ch = In8(Bus + ATA_CYL_HIGH);

  /* differentiate ATA, ATAPI, SATA and SATAPI */
  if (cl == 0x14 && ch == 0xEB) {
    writer.Write(L"NewBoot: PATAPI drive detected.\r\n");
    kATADeviceType = kATADevicePATA_PI;
  }
  if (cl == 0x69 && ch == 0x96) {
    writer.Write(L"NewBoot: SATAPI drive detected.\r\n");
    kATADeviceType = kATADeviceSATA_PI;
  }

  if (cl == 0 && ch == 0) {
    writer.Write(L"NewBoot: PATA drive detected.\r\n");
    kATADeviceType = kATADevicePATA;
  }

  if (cl == 0x3c && ch == 0xc3) {
    writer.Write(L"NewBoot: SATA drive detected.\r\n");
    kATADeviceType = kATADeviceSATA;
  }

  Out8(IO + ATA_REG_CONTROL, 0x02);

  return true;
}

Void boot_ata_read(UInt32 Lba, UInt8 IO, UInt8 Master, CharacterType* Buf,
                SizeT Offset) {
  UInt8 Command = (Master ? 0xE0 : 0xF0);

  Out8(IO + ATA_REG_HDDEVSEL,
       (Command << 4) | (((Lba & 0x0f000000) >> 24) & 0x0f));
  Out8(IO + ATA_REG_SEC_COUNT0, 0x1);
  Out8(IO + ATA_REG_FEATURES, 0);

  Out8(IO + ATA_REG_LBA0, (UInt8)(Lba & 0x000000ff));
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba & 0x0000ff00) >> 8);
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba & 0x00ff0000) >> 16);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  boot_ata_wait_io(IO);

  for (SizeT IndexOff = 0; IndexOff < 256; ++IndexOff) {
    Buf[Offset + IndexOff] = In16(IO + ATA_REG_DATA);
  }
}

Void boot_ata_write(UInt32 Lba, UInt8 IO, UInt8 Master, CharacterType* Buf,
                 SizeT Offset) {
  UInt8 Command = (Master ? 0xE0 : 0xF0);

  Out8(IO + ATA_REG_HDDEVSEL,
       (Command << 4) | (((Lba & 0x0f000000) >> 24) & 0x0f));
  Out8(IO + ATA_REG_SEC_COUNT0, 0x1);
  Out8(IO + ATA_REG_FEATURES, 0);

  Out8(IO + ATA_REG_LBA0, (UInt8)(Lba & 0x000000ff));
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba & 0x0000ff00) >> 8);
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba & 0x00ff0000) >> 16);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  boot_ata_wait_io(IO);

  for (SizeT IndexOff = 0; IndexOff < 256; ++IndexOff) {
    Out16(IO + ATA_REG_DATA, Buf[Offset + IndexOff]);
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
BDeviceATA::BDeviceATA() noexcept {
  if (boot_ata_detected()) return;

  if (boot_ata_init(ATA_PRIMARY_IO, true, this->Leak().mBus,
                    this->Leak().mMaster) ||
      boot_ata_init(ATA_PRIMARY_IO, false, this->Leak().mBus,
                    this->Leak().mMaster) ||
      boot_ata_init(ATA_SECONDARY_IO, true, this->Leak().mBus,
                    this->Leak().mMaster) ||
      boot_ata_init(ATA_SECONDARY_IO, false, this->Leak().mBus,
                    this->Leak().mMaster)) {
    kATADetected = true;

    BTextWriter writer;
    writer.Write(L"NewBoot: Driver: OnLine.\r\n");
  }
}
/**
 * @brief Is ATA detected?
*/
BDeviceATA::operator bool() { return boot_ata_detected(); }

/**
    @brief Read Buf from disk
    @param Sz Sector size
    @param Buf buffer
*/
BDeviceATA& BDeviceATA::Read(CharacterType* Buf, const SizeT& SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  this->Leak().mErr = false;

  if (!Buf || SectorSz < 1) return *this;

  for (SizeT i = 0UL; i < SectorSz; ++i) {
    boot_ata_read(this->Leak().mBase + i, 
                  this->Leak().mBus, this->Leak().mMaster,
               Buf, i);
  }

  return *this;
}

/**
    @brief Write Buf into disk
    @param Sz Sector size
    @param Buf buffer
*/
BDeviceATA& BDeviceATA::Write(CharacterType* Buf, const SizeT& SectorSz) {
  if (!boot_ata_detected()) {
    Leak().mErr = true;
    return *this;
  }

  Leak().mErr = false;

  if (!Buf || SectorSz < 1) return *this;

  SizeT Off = 0UL;

  for (SizeT i = 0UL; i < SectorSz; ++i) {
    boot_ata_write(this->Leak().mBase + i, this->Leak().mBus, this->Leak().mMaster,
                Buf, Off);

    Off += kATASectorSize;
  }

  return *this;
}

/**
 * @brief ATA trait getter.
 * @return BDeviceATA::ATATrait& the drive config.
 */
BDeviceATA::ATATrait& BDeviceATA::Leak() { return mTrait; }
