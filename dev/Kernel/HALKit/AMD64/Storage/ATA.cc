/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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

#if defined(__ATA_PIO__) || defined(__ATA_DMA__)

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
		rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
	auto rdy = rt_in8(IO + ATA_REG_STATUS);

	if ((rdy & ATA_SR_BSY))
		goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
	rdy = rt_in8(IO + ATA_REG_STATUS);

	if (rdy & ATA_SR_ERR)
		return false;

	if (!(rdy & ATA_SR_DRDY))
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

	// Step 1: Wait until drive is not busy
	int timeout = 100000;
	while ((rt_in8(IO + ATA_REG_STATUS) & ATA_SR_BSY) && --timeout)
		;
	if (!timeout)
	{
		kcout << "Timeout waiting for drive to become ready...\r";
		return false;
	}

	// Step 2: Send IDENTIFY command
	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

	// Step 3: Wait for the drive to be ready (not busy)
	drv_std_wait_io(IO);

	// Step 4: Check for errors
	UInt8 status = rt_in8(IO + ATA_REG_STATUS);
	if (status & ATA_SR_ERR)
	{
		kcout << "ATA Error, aborting...\r";
		return false;
	}

	// Step 5: Read IDENTIFY DEVICE response
	for (SizeT indexData = 0UL; indexData < kATADataLen; ++indexData)
	{
		kATAData[indexData] = rt_in16(IO + ATA_REG_DATA);
	}

	// Step 6: Set OutBus & OutMaster correctly
	OutBus	  = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
	OutMaster = (Drive == ATA_MASTER) ? ATA_MASTER : ATA_SLAVE;

#ifdef __ATA_DMA__
	// Step 7: Check if the drive supports DMA
	if (!(kATAData[63] & (1 << 8)) || !(kATAData[88] & 0xFF))
	{
		kcout << "No DMA support...\r";
		ke_panic(RUNTIME_CHECK_FAILED, "No DMA support on necessry disk driver.");

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
		kcout << "DMA Initialization Timeout...\r";
		return false;
	}
#endif // __ATA_DMA__

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

#ifdef __ATA_PIO__
	drv_std_wait_io(IO);
#endif

	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz - 1) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba) & 0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

#ifdef __ATA_PIO__
	rt_out8(ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	drv_std_wait_io(IO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		rt_out16(IO + ATA_REG_DATA, Buf[IndexOff]);
		drv_std_wait_io(IO);
	}
#else
	if (Size > kib_cast(64))
		ke_panic(RUNTIME_CHECK_FAILED, "ATA-DMA only supports < 64kb DMA transfers.");

	Details::PRD* prd = (Details::PRD*)mib_cast(4);
	prd->mAddress	  = (UInt32)(UIntPtr)Buf;
	prd->mByteCount	  = Size;
	prd->mFlags		  = 0x8000;

	rt_out32(IO + 0x04, (UInt32)(UIntPtr)prd);

	rt_out8(IO + 0x00, 0x09); // Start DMA engine
	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_READ_DMA);

	while (rt_in8(ATA_REG_STATUS) & 0x01)
		;
	rt_out8(IO + 0x00, 0x00); // Start DMA engine
#endif // __ATA_PIO__
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

#ifdef __ATA_PIO__
	drv_std_wait_io(IO);
#endif

	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz - 1)) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba) & 0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

#ifdef __ATA_PIO__
	rt_out8(ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	drv_std_wait_io(IO);

	for (SizeT IndexOff = 0; IndexOff < Size; ++IndexOff)
	{
		drv_std_wait_io(IO);
		rt_out16(IO + ATA_REG_DATA, Buf[IndexOff]);
		drv_std_wait_io(IO);
	}
#else
	if (Size > kib_cast(64))
		ke_panic(RUNTIME_CHECK_FAILED, "ATA-DMA only supports < 64kb DMA transfers.");

	Details::PRD* prd = (Details::PRD*)mib_cast(4);
	prd->mAddress	  = (UInt32)(UIntPtr)Buf;
	prd->mByteCount	  = Size;
	prd->mFlags		  = 0x8000;

	rt_out32(IO + 0x04, (UInt32)(UIntPtr)prd);

	rt_out8(IO + 0x00, 0x09); // Start DMA engine
	rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_WRITE_DMA);

	while (rt_in8(ATA_REG_STATUS) & 0x01)
		;
	rt_out8(IO + 0x00, 0x00); // Start DMA engine
#endif // __ATA_PIO__
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

#endif /* ifdef __ATA_PIO__ */
