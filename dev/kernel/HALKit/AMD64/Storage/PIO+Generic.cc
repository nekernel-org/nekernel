/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file PIO+Generic.cc
 * @author Amlal El Mahrouss (amlal@nekernel.org)
 * @brief ATA driver (PIO mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal El Mahrouss
 *
 */

#include <ArchKit/ArchKit.h>
#include <KernelKit/DriveMgr.h>
#include <StorageKit/ATA.h>
#include <modules/ATA/ATA.h>

using namespace Kernel;
using namespace Kernel::HAL;

/// BUGS: 0

#define kATADataLen 256

STATIC Boolean kATADetected                  = false;
STATIC UInt16  kATAIdentifyData[kATADataLen] = {0};
STATIC Char    kATADiskModel[50]             = {"GENERIC PIO"};

static Boolean drv_pio_std_wait_io(UInt16 IO) {
  for (int i = 0; i < 400; i++) rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
  auto stat_rdy = rt_in8(IO + ATA_REG_STATUS);

  if ((stat_rdy & ATA_SR_BSY)) goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
  stat_rdy = rt_in8(IO + ATA_REG_STATUS);

  if (stat_rdy & ATA_SR_ERR) return false;

  if (!(stat_rdy & ATA_SR_DRDY)) goto ATAWaitForIO_Retry2;

  return true;
}

STATIC Void drv_pio_std_select(UInt16 Bus) {
  if (Bus == ATA_PRIMARY_IO)
    rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
  else
    rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean drv_pio_std_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus, UInt8& OutMaster) {
  UInt16 IO = Bus;

  NE_UNUSED(Drive);

  drv_pio_std_select(IO);

  // Bus init, NEIN bit.
  rt_out8(IO + ATA_REG_NEIN, 1);

  // identify until it's good.
ATAInit_Retry:
  auto stat_rdy = rt_in8(IO + ATA_REG_STATUS);

  if (stat_rdy & ATA_SR_ERR) {
    return false;
  }

  if ((stat_rdy & ATA_SR_BSY)) goto ATAInit_Retry;

  OutBus    = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
  OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

  drv_pio_std_select(IO);

  rt_out8(OutBus + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ));

  /// fetch serial info
  /// model, speed, number of sectors...

  for (SizeT i = 0ul; i < kATADataLen; ++i) {
    kATAIdentifyData[i] = rt_in16(OutBus + ATA_REG_DATA);
  }

  for (Int32 i = 0; i < 20; i++) {
    kATADiskModel[i * 2]     = (kATAIdentifyData[27 + i] >> 8) & 0xFF;
    kATADiskModel[i * 2 + 1] = kATAIdentifyData[27 + i] & 0xFF;
  }

  kATADiskModel[40] = '\0';

  (Void)(kout << "Drive Model: " << kATADiskModel << kendl);

  return true;
}

Void drv_pio_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size) {
  Lba /= SectorSz;

  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  drv_pio_std_wait_io(IO);
  drv_pio_std_select(IO);

  rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

  rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz) / SectorSz));

  rt_out8(IO + ATA_REG_LBA0, (Lba) & 0xFF);
  rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

  rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ));

  for (SizeT IndexOff = 0; IndexOff < Size; IndexOff += 2) {
    drv_pio_std_wait_io(IO);

    auto in = rt_in16(IO + ATA_REG_DATA);

    Buf[IndexOff]     = in & 0xFF;
    Buf[IndexOff + 1] = (in >> 8) & 0xFF;
  }
}

Void drv_pio_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size) {
  Lba /= SectorSz;

  UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

  drv_pio_std_wait_io(IO);
  drv_pio_std_select(IO);

  rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

  rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz) / SectorSz));

  rt_out8(IO + ATA_REG_LBA0, (Lba) & 0xFF);
  rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
  rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
  rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

  rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRQ));

  for (SizeT IndexOff = 0; IndexOff < Size; IndexOff += 2) {
    drv_pio_std_wait_io(IO);

    UInt8  low    = (UInt8) Buf[IndexOff];
    UInt8  high   = (IndexOff + 1 < Size) ? (UInt8) Buf[IndexOff + 1] : 0;
    UInt16 packed = (high << 8) | low;

    rt_out16(IO + ATA_REG_DATA, packed);
  }
}

/// @brief is ATA detected?
Boolean drv_pio_std_detected(Void) {
  return kATADetected;
}

/***
   @brief Getter, gets the number of sectors inside the drive.
 */
SizeT drv_pio_get_sector_count() {
  return (kATAIdentifyData[61] << 16) | kATAIdentifyData[60];
}

/// @brief Get the drive size.
SizeT drv_pio_get_size() {
  return (drv_pio_get_sector_count()) * kATASectorSize;
}

namespace Kernel {
/// @brief Initialize an PIO device (StorageKit function)
/// @param is_master is the current PIO master?
/// @return [io:master] for PIO device.
BOOL sk_init_ata_device(BOOL is_master, UInt16& io, UInt8& master) {
  return drv_pio_std_init(ATA_SECONDARY_IO, is_master, io, master);
}

/// @brief Implementation details namespace.
namespace Detail {
  /// @brief Read PIO device.
  /// @param self device
  /// @param mnt mounted disk.
  STATIC Void sk_io_read_pio(DeviceInterface<MountpointInterface*>* self,
                             MountpointInterface*                   mnt) {
    ATADeviceInterface* dev = (ATADeviceInterface*) self;

    err_global_get() = kErrorDisk;

    if (!dev) return;

    auto disk = mnt->GetAddressOf(dev->GetIndex());

    if (!disk) return;

    err_global_get() = kErrorSuccess;

    drv_pio_std_read(disk->fPacket.fPacketLba, dev->GetIO(), dev->GetMaster(),
                     (Char*) disk->fPacket.fPacketContent, kATASectorSize,
                     disk->fPacket.fPacketSize);
  }

  /// @brief Write PIO device.
  /// @param self device
  /// @param mnt mounted disk.
  STATIC Void sk_io_write_pio(DeviceInterface<MountpointInterface*>* self,
                              MountpointInterface*                   mnt) {
    ATADeviceInterface* dev = (ATADeviceInterface*) self;

    err_global_get() = kErrorDisk;

    if (!dev) return;

    auto disk = mnt->GetAddressOf(dev->GetIndex());

    if (!disk) return;

    err_global_get() = kErrorSuccess;

    drv_pio_std_write(disk->fPacket.fPacketLba, dev->GetIO(), dev->GetMaster(),
                      (Char*) disk->fPacket.fPacketContent, kATASectorSize,
                      disk->fPacket.fPacketSize);
  }
}  // namespace Detail

/// @brief Acquires a new PIO device with drv_index in mind.
/// @param drv_index The drive index to assign.
/// @return A wrapped device interface if successful, or error code.
ErrorOr<ATADeviceInterface> sk_acquire_ata_device(Int32 drv_index) {
  /// here we don't check if we probed ATA, since we'd need to grab IO after that.
  ATADeviceInterface device(Detail::sk_io_read_pio, Detail::sk_io_write_pio);

  device.SetIndex(drv_index);

  return ErrorOr<ATADeviceInterface>(device);
}
}  // namespace Kernel

#ifdef __ATA_PIO__

Void drv_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size) {
  drv_pio_std_read(Lba, IO, Master, Buf, SectorSz, Size);
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size) {
  drv_pio_std_write(Lba, IO, Master, Buf, SectorSz, Size);
}

SizeT drv_std_get_size() {
  return drv_pio_get_size();
}

SizeT drv_std_get_sector_count() {
  return drv_pio_get_sector_count();
}

Boolean drv_std_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus, UInt8& OutMaster) {
  return drv_pio_std_init(Bus, Drive, OutBus, OutMaster);
}

#endif