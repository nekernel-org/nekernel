/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/**
 * @file ATA.cxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief ATA driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <BootKit/Arch/ATA.hxx>
#include <BootKit/BootKit.hxx>

/// bugs: 0

static Boolean kATADetected = false;
static Int32 kATADeviceType = kATADeviceCount;

void ATASelect(UInt8 Bus, Boolean isMaster) {
  if (Bus == ATA_PRIMARY)
    Out8(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
  else
    Out8(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
}

Boolean ATAInitDriver(UInt8 Bus, UInt8 Drive, UInt16& OutBus,
                      Boolean& OutMaster) {
  BTextWriter writer;

  UInt16 IO = (Bus == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

  ATASelect(Bus, Drive);

  Out8(IO + ATA_REG_SEC_COUNT0, 0);
  Out8(IO + ATA_REG_LBA0, 0);
  Out8(IO + ATA_REG_LBA1, 0);
  Out8(IO + ATA_REG_LBA2, 0);

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  UInt8 status = In8(IO + ATA_REG_STATUS);

  if (status & ATA_SR_ERR) {
#ifdef __DEBUG__
    writer.WriteString(L"HCoreLdr: Init ATA: Bad Drive!\r\n");
#endif  // ifdef __DEBUG__
    return false;
  }

  OutBus = (Bus == ATA_PRIMARY) ? BATADevice::kPrimary : BATADevice::kSecondary;
  OutMaster = (Bus == ATA_PRIMARY);

  unsigned cl = In16(Bus + ATA_CYL_LOW); /* get the "signature bytes" */
  unsigned ch = In16(Bus + ATA_CYL_HIGH);

  /* differentiate ATA, ATAPI, SATA and SATAPI */
  if (cl == 0x14 && ch == 0xEB) {
    writer.WriteString(L"HCoreLdr: PATAPI drive detected.\r\n");
    kATADeviceType = kATADevicePATA_PI;
  }
  if (cl == 0x69 && ch == 0x96) {
    writer.WriteString(L"HCoreLdr: SATAPI drive detected.\r\n");
    kATADeviceType = kATADeviceSATA_PI;
  }
  if (cl == 0 && ch == 0) {
    writer.WriteString(L"HCoreLdr: PATA drive detected.\r\n");
    kATADeviceType = kATADevicePATA;
  }
  if (cl == 0x3c && ch == 0xc3) {
    writer.WriteString(L"HCoreLdr: SATA drive detected.\r\n");
    kATADeviceType = kATADeviceSATA;
  }

  return true;
}

/*** @brief
 * This polls the ATA drive.
 */
void ATAWait(UInt16 IO) {
  for (int i = 0; i < 4000; i++) In8(IO + ATA_REG_ALT_STATUS);
}

void ATAPoll(UInt16 IO) { ATAWait(IO); }

Void ATAReadLba(UInt32 Lba, UInt8 Bus, Boolean Master, CharacterType* Buf,
                SizeT Offset) {
  UInt16 IO = Bus;

  ATASelect(IO + ATA_REG_HDDEVSEL,
            (Master ? ATA_MASTER : ATA_SLAVE) | Lba >> 24 & 0xF);

  ATASelect(IO + 1, 0);

  Out8(IO + ATA_REG_SEC_COUNT0, 1);

  Out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  ATAPoll(IO);

  Buf[Offset] = In16(IO + ATA_REG_DATA);

  ATAWait(IO);
}

Void ATAWriteLba(UInt32 Lba, UInt8 Bus, Boolean Master, wchar_t* Buf,
                 SizeT Offset) {
  UInt16 IO = Bus;

  ATASelect(IO + ATA_REG_HDDEVSEL,
            (Master ? ATA_MASTER : ATA_SLAVE) | Lba >> 24 & 0xF);

  ATASelect(IO + 1, 0);

  Out8(IO + ATA_REG_SEC_COUNT0, 1);

  Out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  Out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  Out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  Out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  ATAPoll(IO);

  Out16(IO + ATA_REG_DATA, Buf[Offset]);

  ATAWait(IO);
}

Boolean ATAIsDetected(Void) { return kATADetected; }

/***
 *
 *
 * ATA Device class.
 *
 *
 */

/**
 * @brief ATA Device constructor.
 * @param void none.
 */
BATADevice::BATADevice() noexcept {
  if (ATAIsDetected()) return;

  if (ATAInitDriver(ATA_PRIMARY, true, this->Leak().mBus,
                    this->Leak().mMaster) ||
      ATAInitDriver(ATA_PRIMARY, false, this->Leak().mBus,
                    this->Leak().mMaster) ||
      ATAInitDriver(ATA_SECONDARY, true, this->Leak().mBus,
                    this->Leak().mMaster) ||
      ATAInitDriver(ATA_SECONDARY, false, this->Leak().mBus,
                    this->Leak().mMaster)) {
    kATADetected = true;

    BTextWriter writer;
    writer.WriteString(L"BATADevice::BATADevice: OnLine\r\n");
  }
}

/**
    @brief Read Buf from disk
    @param Sz Sector size
    @param Buf buffer
*/
BATADevice& BATADevice::Read(CharacterType* Buf, const SizeT& Sz) {
  if (!ATAIsDetected()) return *this;

  if (!Buf || Sz < 1) return *this;

  for (SizeT i = 0UL; i < Sz; ++i) {
    ATAReadLba(this->Leak().mBase + i, this->Leak().mBus, this->Leak().mMaster,
               Buf, i);
  }

  return *this;
}

/**
    @brief Write Buf into disk
    @param Sz Sector size
    @param Buf buffer
*/
BATADevice& BATADevice::Write(CharacterType* Buf, const SizeT& Sz) {
  if (!ATAIsDetected()) return *this;

  if (!Buf || Sz < 1) return *this;

  SizeT Off = 0UL;

  for (SizeT i = 0UL; i < Sz; ++i) {
    ATAWriteLba(this->Leak().mBase + i, this->Leak().mBus, this->Leak().mMaster,
                Buf, Off);

    Off += kATASectorSz;
  }

  return *this;
}

/**
 * @brief ATA Config getter.
 * @return BATADevice::ATATraits& the drive config.
 */
BATADevice::ATATraits& BATADevice::Leak() { return mTraits; }
