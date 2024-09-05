/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/**
 * @file ATA-PIO.cxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief ATA driver (PIO mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) ZKA Technologies
 *
 */

#include <Modules/ATA/ATA.hxx>
#include <ArchKit/ArchKit.hxx>

#ifdef __ATA_PIO__

using namespace Kernel;
using namespace Kernel::HAL;

/// bugs: 0

#define kATADataLen 256

STATIC Boolean kATADetected			 = false;
STATIC Int32   kATADeviceType		 = kATADeviceCount;
STATIC Char	   kATAData[kATADataLen] = {0};

Boolean drv_std_wait_io(UInt16 IO)
{
	for (int i = 0; i < 400; i++)
		In8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
	auto statRdy = In8(IO + ATA_REG_STATUS);

	if ((statRdy & ATA_SR_BSY))
		goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
	statRdy = In8(IO + ATA_REG_STATUS);

	if (statRdy & ATA_SR_ERR)
		return false;

	if (!(statRdy & ATA_SR_DRDY))
		goto ATAWaitForIO_Retry2;

	return true;
}

Void drv_std_select(UInt16 Bus)
{
	if (Bus == ATA_PRIMARY_IO)
		Out8(Bus + ATA_REG_HDDEVSEL, ATA_PRIMARY_SEL);
	else
		Out8(Bus + ATA_REG_HDDEVSEL, ATA_SECONDARY_SEL);
}

Boolean drv_std_init(UInt16 Bus, UInt8 Drive, UInt16& OutBus, UInt8& OutMaster)
{
	UInt16 IO = Bus;

	drv_std_select(IO);

	kcout << "Initializing drive...\r";

ATAInit_Retry:
	// Bus init, NEIN bit.
	Out8(IO + ATA_REG_NEIN, 1);

	// identify until it's good

	auto statRdy = In8(IO + ATA_REG_STATUS);

	if (statRdy & ATA_SR_ERR)
	{
		kcout << "Failing drive...\r";

		return false;
	}

	if ((statRdy & ATA_SR_BSY))
	{
		kcout << "Retrying...";
		goto ATAInit_Retry;
	}

	Out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

	/// fetch serial info
	/// model, speed, number of sectors...

	drv_std_wait_io(IO);

	for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData)
	{
		kATAData[indexData] = In16(IO + ATA_REG_DATA);
	}

	OutBus = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

	OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

	kcout << "Created IDE module.\r";

	return true;
}

Void drv_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_wait_io(IO);
	drv_std_select(IO);

	Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	Out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz) / SectorSz));

	Out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	Out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	Out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	Out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	Out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	drv_std_wait_io(IO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		Buf[IndexOff] = In16(IO + ATA_REG_DATA);
		drv_std_wait_io(IO);
	}

	drv_std_wait_io(IO);
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_wait_io(IO);
	drv_std_select(IO);

	Out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	Out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz)) / SectorSz));

	Out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	Out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	Out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	Out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	Out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	drv_std_wait_io(IO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		Out16(IO + ATA_REG_DATA, Buf[IndexOff]);
		drv_std_wait_io(IO);
	}

	drv_std_wait_io(IO);
}

/// @brief is ATA detected?
Boolean drv_std_detected(Void)
{
	return kATADetected;
}

/***
	@brief Getter, gets the number of sectors inside the drive.
*/
Kernel::SizeT drv_std_get_sector_count()
{
	return (kATAData[61] << 16) | kATAData[60];
}

/// @brief Get the drive size.
Kernel::SizeT drv_std_get_drv_size()
{
	return drv_std_get_sector_count() * kATASectorSize;
}

#endif /* ifdef __ATA_PIO__ */
