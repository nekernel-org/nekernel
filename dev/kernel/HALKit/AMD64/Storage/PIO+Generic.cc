/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file PIO+Generic.cc
 * @author Amlal EL Mahrouss (amlal@nekernel.org)
 * @brief ATA driver (PIO mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal EL Mahrouss
 *
 */

#include <modules/ATA/ATA.h>
#include <ArchKit/ArchKit.h>

#ifdef __ATA_PIO__

using namespace Kernel;
using namespace Kernel::HAL;

/// BUGS: 0

#define kATADataLen 512

STATIC Boolean kATADetected			 = false;
STATIC Int32   kATADeviceType		 = kATADeviceCount;
STATIC Char	   kATAData[kATADataLen] = {0};
STATIC Char	   kCurrentDiskModel[50] = {"UNKNOWN PIO DRIVE"};

Boolean drv_std_wait_io(UInt16 IO)
{
	for (int i = 0; i < 400; i++)
		rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
	auto stat_rdy = rt_in8(IO + ATA_REG_STATUS);

	if ((stat_rdy & ATA_SR_BSY))
		goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
	stat_rdy = rt_in8(IO + ATA_REG_STATUS);

	if (stat_rdy & ATA_SR_ERR)
		return false;

	if (!(stat_rdy & ATA_SR_DRDY))
		goto ATAWaitForIO_Retry2;

	return true;
}

Void drv_std_select(UInt16 Bus)
{
	if (Bus == ATA_PRIMARY_IO)
		rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
	else
		rt_out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean drv_std_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus, UInt8& OutMaster)
{
	UInt16 IO = Bus;

	drv_std_select(IO);

	// Bus init, NEIN bit.
	rt_out8(IO + ATA_REG_NEIN, 1);

	// identify until it's good.
ATAInit_Retry:
	auto stat_rdy = rt_in8(IO + ATA_REG_STATUS);

	if (stat_rdy & ATA_SR_ERR)
	{
		return false;
	}

	if ((stat_rdy & ATA_SR_BSY))
		goto ATAInit_Retry;

	OutBus	  = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
	OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

	rt_out8(OutBus + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

	drv_std_wait_io(IO);

	/// fetch serial info
	/// model, speed, number of sectors...

	for (SizeT i = 0ul; i < kATADataLen; ++i)
	{
		kATAData[i] = HAL::rt_in16(OutBus + ATA_REG_DATA);
	}

	for (Int32 i = 0; i < 20; i++)
	{
		kCurrentDiskModel[i * 2]	 = kATAData[27 + i] >> 8;
		kCurrentDiskModel[i * 2 + 1] = kATAData[27 + i + 1] & 0xFF;
	}

	kCurrentDiskModel[40] = '\0';

	kout << "Detect: /dev/ata0" << kendl;

	kout << "Drive Model: " << kCurrentDiskModel << kendl;

	return true;
}

Void drv_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_wait_io(IO);
	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		Buf[IndexOff] = HAL::rt_in16(IO + ATA_REG_DATA);
	}
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_wait_io(IO);
	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz)) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		rt_out16(IO + ATA_REG_DATA, Buf[IndexOff]);
	}
}

/// @brief is ATA detected?
Boolean drv_std_detected(Void)
{
	return kATADetected;
}

/***
	 @brief Getter, gets the number of sectors inside the drive.
 */
SizeT drv_get_sector_count()
{
	return (kATAData[61] << 16) | kATAData[60];
}

/// @brief Get the drive size.
SizeT drv_get_size()
{
	return (drv_get_sector_count()) * kATASectorSize;
}

#endif /* ifdef __ATA_PIO__ */