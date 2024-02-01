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

static Boolean kATADetected = false;

extern "C" void out8(UInt16 port, UInt8 value) {
  asm volatile("outb %%al, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" void out16(UInt16 port, UInt16 value) {
  asm volatile("outw %%ax, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" void out32(UInt16 port, UInt32 value) {
  asm volatile("outl %%eax, %1" : : "a"(value), "Nd"(port) : "memory");
}

extern "C" UInt8 in8(UInt16 port) {
  UInt8 value = 0UL;
  asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

extern "C" UInt16 in16(UInt16 port) {
  UInt16 value = 0UL;
  asm volatile("inw %1, %%ax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

extern "C" UInt32 in32(UInt16 port) {
  UInt32 value = 0UL;
  asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

void ATASelect(UInt8 Bus, Boolean isMaster) {
  if (Bus == ATA_PRIMARY)
    out8(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
  else
    out8(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL,
         isMaster ? ATA_PRIMARY_SEL : ATA_SECONDARY_SEL);
}

Boolean ATAInitDriver(UInt8 Bus, UInt8 Drive) {
  BTextWriter writer;

  UInt16 IO = (Bus == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

  ATASelect(Bus, Drive);

  out8(IO + ATA_REG_SEC_COUNT0, 0);
  out8(IO + ATA_REG_LBA0, 0);
  out8(IO + ATA_REG_LBA1, 0);
  out8(IO + ATA_REG_LBA2, 0);

  out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  UInt8 status = in8(IO + ATA_REG_STATUS);
  writer.WriteString(L"HCoreLdr: Init ATA: Checking status...\r\n");

  if (status) {
    while ((status = in8(IO + ATA_REG_STATUS) & ATA_SR_BSY))
      ;

    if (status & ATA_REG_ERROR) {
#ifdef __DEBUG__
      writer.WriteString(L"HCoreLdr: Init ATA: Bad Drive!\r\n");
#endif  // ifdef __DEBUG__
      return false;
    }

#ifdef __DEBUG__
    writer.WriteString(L"HCoreLdr: Init ATA: OnLine!\r\n");
#endif  // ifdef __DEBUG__

    kATADetected = true;
    return status;
  }

#ifdef __DEBUG__
  writer.WriteString(L"HCoreLdr: Init ATA: Not detected!\r\n");
#endif  // ifdef __DEBUG__
  return false;
}

void ATAWait(UInt16 IO) {
  for (int i = 0; i < 4000; i++) in8(IO + ATA_REG_ALT_STATUS);
}

void ATAPoll(UInt16 IO) { ATAWait(IO); }

UInt16 ATAReadLba(UInt32 Lba, UInt8 Bus, Boolean Master) {
  UInt16 IO = Bus;
  ATASelect(IO, Master ? ATA_MASTER : ATA_SLAVE);

  out8(IO + ATA_REG_LBA5, (UInt8)(Lba >> 24) & 0xF);

  out8(IO + ATA_REG_SEC_COUNT0, Lba / 512);

  out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  ATAPoll(IO);

  UInt16 data = in16(IO + ATA_REG_DATA);

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);

  return data;
}

Void ATAWriteLba(UInt16 Byte, UInt32 Lba, UInt8 Bus, Boolean Master) {
  UInt16 IO = Bus;
  ATASelect(IO, Master ? ATA_MASTER : ATA_SLAVE);

  out8(IO + ATA_REG_LBA5, (UInt8)(Lba >> 24) & 0xF);

  out8(IO + ATA_REG_SEC_COUNT0, Lba / 512);

  out8(IO + ATA_REG_LBA0, (UInt8)Lba);
  out8(IO + ATA_REG_LBA1, (UInt8)(Lba >> 8));
  out8(IO + ATA_REG_LBA2, (UInt8)(Lba >> 16));

  out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);  // TODO: support DMA

  ATAPoll(IO);

  out32(IO + ATA_REG_DATA, Byte);

  while ((in8(ATA_COMMAND(IO))) & 0x88) ATAWait(IO);
}

Boolean ATAIsDetected(Void) { return kATADetected; }

/**
 * @brief Init ATA driver.
 * @param void none.
 */
ATAHelper::ATAHelper() noexcept {
  if (ATAIsDetected()) return;

  ATAInitDriver(ATA_PRIMARY, true);
  ATAInitDriver(ATA_PRIMARY, false);

  ATAInitDriver(ATA_SECONDARY, true);
  ATAInitDriver(ATA_SECONDARY, false);

  BTextWriter writer;
  writer.WriteString(L"KeInitATA: Init: Done\r\n");
}

ATAHelper& ATAHelper::Read(CharacterType* Buf, const SizeT& Sz) {
  if (!Buf || Sz < 1) return *this;

  for (SizeT i = 0UL; i < Sz; ++i) {
    Buf[i] = ATAReadLba(this->Traits().mBase + i, this->Traits().mBus,
                        this->Traits().mMaster);
  }
  return *this;
}

ATAHelper& ATAHelper::Write(CharacterType* Buf, const SizeT& Sz) {
  if (!Buf || Sz < 1) return *this;

  for (SizeT i = 0UL; i < Sz; ++i) {
    ATAWriteLba(Buf[i], this->Traits().mBase + i, this->Traits().mBus,
                this->Traits().mMaster);
  }
  return *this;
}

ATAHelper::ATATraits& ATAHelper::Traits() { return mTraits; }
