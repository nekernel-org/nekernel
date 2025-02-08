/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file ATA-PIO.cc
 * @author Amlal EL Mahrouss (amlalelmahrouss@icloud.com)
 * @brief ATA driver (PIO mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal EL Mahrouss
 *
 */

#include <Mod/ATA/ATA.h>
#include <ArchKit/ArchKit.h>

#if defined(__ATA_DMA__)

#define kATADataLen 256

using namespace Kernel;
using namespace Kernel::HAL;

/// BUGS: 0

STATIC Boolean kATADetected			 = false;
STATIC Int32   kATADeviceType		 = kATADeviceCount;
STATIC Char	   kATAData[kATADataLen] = {0};

Boolean drv_std_wait_io(UInt16 IO)
{
	for (int i = 0; i < 400; i++)
		rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
	auto statRdy = rt_in8(IO + ATA_REG_STATUS);

	if ((statRdy & ATA_SR_BSY))
		goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
	statRdy = rt_in8(IO + ATA_REG_STATUS);

	if (statRdy & ATA_SR_ERR)
		return false;

	if (!(statRdy & ATA_SR_DRDY))
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
	if (drv_std_detected())
		return true;

	UInt16 IO = Bus;

	drv_std_select(IO);

	// Bus init, NEIN bit.
	rt_out8(IO + ATA_REG_NEIN, 1);

	// identify until it's good.
ATAInit_Retry:
	auto statRdy = rt_in8(IO + ATA_REG_STATUS);

	if (statRdy & ATA_SR_ERR)
	{
		return false;
	}

	if ((statRdy & ATA_SR_BSY))
		goto ATAInit_Retry;

	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

	/// fetch serial info
	/// model, speed, number of sectors...

	drv_std_wait_io(IO);

	for (SizeT indexData = 0ul; indexData < kATADataLen; ++indexData)
	{
		kATAData[indexData] = Kernel::HAL::rt_in16(IO + ATA_REG_DATA);
	}

	OutBus	  = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
	OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

	return true;
#ifdef __ATA_DMA__
	// Step 7: Check if the drive supports DMA
	if (!(kATAData[63] & (1 << 8)) || !(kATAData[88] & 0xFF))
	{
		kout << "No DMA support...\r";
		ke_panic(RUNTIME_CHECK_BOOTSTRAP, "No DMA support on necessary disk driver.");

		return false;
	}

	// Step 8: Enable DMA Mode
	rt_out8(IO + ATA_REG_FEATURES, 0x03);				 // Enable DMA mode
	rt_out8(IO + ATA_REG_COMMAND, ATA_REG_SET_FEATURES); // Send set features command

	// Step 9: Wait for drive to acknowledge DMA setting
	timeout = 100000;
	while (!(rt_in8(IO + ATA_REG_STATUS) & ATA_SR_DRDY) && --timeout)
		;
	if (!timeout)
	{
		kout << "DMA Initialization Timeout...\r";
		return false;
	}
#endif // __ATA_DMA__

	kout << "ATA is enabled now.\r";

	return YES;
}

namespace Details
{
	using namespace Kernel;

	struct __attribute__((packed, aligned(4))) PRD final
	{
		UInt32 mAddress;
		UInt16 mByteCount;
		UInt16 mFlags;
	};
} // namespace Details

Void drv_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz - 1) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	if (Size > kib_cast(64))
		ke_panic(RUNTIME_CHECK_FAILED, "ATA-DMA only supports < 64kb DMA transfers.");

	Details::PRD* prd = new Details::PRD();
	prd->mAddress	  = (UInt32)(UIntPtr)Buf;
	prd->mByteCount	  = Size;
	prd->mFlags		  = 0x8000;

	rt_out32(IO + 0x04, (UInt32)(UIntPtr)prd);

	rt_out8(IO + 0x00, 0x09); // Start DMA engine
	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_DMA);

	while (rt_in8(ATA_REG_STATUS) & 0x01)
		;
	rt_out8(IO + 0x00, 0x00); // Stop DMA engine

	delete prd;
	prd = nullptr;
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz - 1)) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	if (Size > kib_cast(64))
		ke_panic(RUNTIME_CHECK_FAILED, "ATA-DMA only supports < 64kb DMA transfers.");

	Details::PRD* prd = new Details::PRD();
	prd->mAddress	  = (UInt32)(UIntPtr)Buf;
	prd->mByteCount	  = Size;
	prd->mFlags		  = 0x8000;

	rt_out32(IO + 0x04, (UInt32)(UIntPtr)prd);

	rt_out8(IO + 0x00, 0x09); // Start DMA engine
	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_DMA);

	while (rt_in8(ATA_REG_STATUS) & 0x01)
		;

	rt_out8(IO + 0x00, 0x00); // Stop DMA engine

	delete prd;
	prd = nullptr;
}

/// @brief is ATA detected?
Boolean drv_std_detected(Void)
{
	return kATADetected;
}

/***
	@brief Getter, gets the number of sectors inside the drive.
*/
Kernel::SizeT drv_get_sector_count()
{
	return (kATAData[61] << 16) | kATAData[60];
}

/// @brief Get the drive size.
Kernel::SizeT drv_get_size()
{
	return (drv_get_sector_count()) * kATASectorSize;
}

#endif /* ifdef __ATA_DMA__ */
