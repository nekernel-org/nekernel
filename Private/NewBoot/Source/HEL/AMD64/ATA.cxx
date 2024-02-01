/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/Arch/ATA.hxx>
#include <BootKit/BootKit.hxx>

/***********************************************************************************/
/// Provide some useful processor features.
/***********************************************************************************/

static Boolean kATADetected = false;

void ATASelect(UInt8 Bus, Boolean isMaster) {
  if (Bus == ATA_PRIMARY)
    out8(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
  else
    out8(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
}

Boolean ATAInitDriver(UInt8 Bus, UInt8 Drive, UInt16& OutBus,
                      Boolean& OutMaster) {
  BTextWriter writer;

  UInt16 IO = (Bus == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

  ATASelect(Bus, Drive);

  out8(IO + ATA_REG_SEC_COUNT0, 0);
  out8(IO + ATA_REG_LBA0, 0);
  out8(IO + ATA_REG_LBA1, 0);
  out8(IO + ATA_REG_LBA2, 0);

  out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  UInt8 status = in8(IO + ATA_REG_STATUS);

  if (status & ATA_SR_ERR) {
#ifdef __DEBUG__
    writer.WriteString(L"HCoreLdr: Init ATA: Bad Drive!\r\n");
#endif  // ifdef __DEBUG__
    return false;
  }

  OutBus = (Bus == ATA_PRIMARY) ? BATADevice::kPrimary : BATADevice::kSecondary;
  OutMaster = (Bus == ATA_PRIMARY);

  char cl = in8(Bus + ATA_CYL_LOW); /* get the "signature bytes" */
  char ch = in8(Bus + ATA_CYL_HIGH);

  /* differentiate ATA, ATAPI, SATA and SATAPI */
  if (cl == 0x14 && ch == 0xEB)
    writer.WriteString(L"HCoreLdr: PATAPI detected.\r\n");
  if (cl == 0x69 && ch == 0x96)
    writer.WriteString(L"HCoreLdr: SATAPI detected.\r\n");
  if (cl == 0 && ch == 0) writer.WriteString(L"HCoreLdr: PATA detected.\r\n");
  if (cl == 0x3c && ch == 0xc3)
    writer.WriteString(L"HCoreLdr: SATA detected.\r\n");

  return true;
}

/*** @brief
 * This polls the ATA drive.
 */
void ATAWait(UInt16 IO) {
  for (int i = 0; i < 4000; i++) in8(IO + ATA_REG_ALT_STATUS);
}

void ATAPoll(UInt16 IO) { ATAWait(IO); }

UInt16 ATAReadLba(UInt32 Lba, UInt8 Bus, Boolean Master) {
  UInt16 IO = Bus;
  ATASelect(IO, Master ? ATA_MASTER : ATA_SLAVE);

  out8(IO + ATA_REG_SEC_COUNT0, Lba / 512);

  out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);

  auto chr = in8(IO + ATA_REG_DATA);

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);
  return chr;
}

Void ATAWriteLba(UInt16 Byte, UInt32 Lba, UInt8 Bus, Boolean Master) {
  UInt16 IO = Bus;
  ATASelect(IO, Master ? ATA_MASTER : ATA_SLAVE);

  out8(IO + ATA_REG_SEC_COUNT0, Lba / 512);

  out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);  // TODO: support DMA

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);

  out32(IO + ATA_REG_DATA, Byte);

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);
}

Boolean ATAIsDetected(Void) { return kATADetected; }

/**
 * @brief Init ATA driver.
 * @param void none.
 */
BATADevice::BATADevice() noexcept {
  if (ATAIsDetected()) return;

  if (ATAInitDriver(ATA_PRIMARY, true, this->Traits().mBus,
                    this->Traits().mMaster) ||
      ATAInitDriver(ATA_PRIMARY, false, this->Traits().mBus,
                    this->Traits().mMaster) ||
      ATAInitDriver(ATA_SECONDARY, true, this->Traits().mBus,
                    this->Traits().mMaster) ||
      ATAInitDriver(ATA_SECONDARY, false, this->Traits().mBus,
                    this->Traits().mMaster)) {
    kATADetected = true;

    BTextWriter writer;
    writer.WriteString(L"BATADevice::BATADevice: OnLine\r\n");
  }
}

BATADevice& BATADevice::Read(CharacterType* Buf, const SizeT& Sz) {
  if (!ATAIsDetected()) return *this;

  if (!Buf || Sz < 1) return *this;

  for (SizeT i = 0UL; i < Sz; ++i) {
    Buf[i] = ATAReadLba(this->Traits().mBase + i, this->Traits().mBus,
                        this->Traits().mMaster);
  }
  return *this;
}

BATADevice& BATADevice::Write(CharacterType* Buf, const SizeT& Sz) {
  if (!ATAIsDetected()) return *this;

  if (!Buf || Sz < 1) return *this;

  for (SizeT i = 0UL; i < Sz; ++i) {
    ATAWriteLba(Buf[i], this->Traits().mBase + i, this->Traits().mBus,
                this->Traits().mMaster);
  }
  return *this;
}

BATADevice::ATATraits& BATADevice::Traits() { return mTraits; }
