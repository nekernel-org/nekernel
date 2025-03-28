/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file ATA-PIO.cc
 * @author Amlal EL Mahrouss (amlal@nekernel.org)
 * @brief ATA driver (DMA mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal EL Mahrouss
 *
 */

#include <modules/ATA/ATA.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/PCI/Iterator.h>

#if defined(__ATA_DMA__)

#define kATADataLen (256)

using namespace Kernel;
using namespace Kernel::HAL;

/// BUGS: 0

STATIC Boolean kATADetected			 = false;
STATIC Int32   kATADeviceType		 = kATADeviceCount;
STATIC Char	   kATAData[kATADataLen] = {0};
STATIC Kernel::PCI::Device kATADevice;
STATIC Char				 kCurrentDiskModel[50] = {"UNKNOWN DMA DRIVE"};

Boolean drv_std_wait_io(UInt16 IO)
{
	for (int i = 0; i < 400; i++)
		rt_in8(IO + ATA_REG_STATUS);

ATAWaitForIO_Retry:
	auto status_rdy = rt_in8(IO + ATA_REG_STATUS);

	if ((status_rdy & ATA_SR_BSY))
		goto ATAWaitForIO_Retry;

ATAWaitForIO_Retry2:
	status_rdy = rt_in8(IO + ATA_REG_STATUS);

	if (status_rdy & ATA_SR_ERR)
		return false;

	if (!(status_rdy & ATA_SR_DRDY))
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
	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < NE_BUS_COUNT; ++device_index)
	{
		kATADevice = iterator[device_index].Leak(); // And then leak the reference.

		// if SATA and then interface is AHCI...
		if (kATADevice.Subclass() == 0x01)
		{
			UInt16 IO = Bus;

			drv_std_select(IO);

			// Bus init, NEIN bit.
			rt_out8(IO + ATA_REG_NEIN, 1);

			// identify until it's good.
		ATAInit_Retry:
			auto status_rdy = rt_in8(IO + ATA_REG_STATUS);

			if (status_rdy & ATA_SR_ERR)
			{
				return false;
			}

			if ((status_rdy & ATA_SR_BSY))
				goto ATAInit_Retry;

			rt_out8(IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

			/// fetch serial info
			/// model, speed, number of sectors...

			drv_std_wait_io(IO);

			for (SizeT i = 0ul; i < kATADataLen; ++i)
			{
				drv_std_wait_io(IO);
				kATAData[i] = Kernel::HAL::rt_in16(IO + ATA_REG_DATA);
				drv_std_wait_io(IO);
			}

			for (SizeT i = 0; i < 40; i += 2)
			{
				kCurrentDiskModel[i * 2]	 = kATAData[27 + i * 2] >> 8;
				kCurrentDiskModel[i * 2 + 1] = kATAData[27 + i * 2] & 0xFF;
			}

			kCurrentDiskModel[40] = '\0';

			kout << "Drive Model: " << kCurrentDiskModel << kendl;

			OutBus	  = (Bus == ATA_PRIMARY_IO) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
			OutMaster = (Bus == ATA_PRIMARY_IO) ? ATA_MASTER : ATA_SLAVE;

			return YES;
		}
	}

	ke_panic(RUNTIME_CHECK_BOOTSTRAP, "Invalid ATA DMA driver, not detected");

	return NO;
}

namespace Kernel::Detail
{
	struct PRDEntry
	{
		UInt32 mAddress;
		UInt16 mByteCount;
		UInt16 mFlags;
	};
} // namespace Kernel::Detail

static UIntPtr kReadAddr  = mib_cast(2);
static UIntPtr kWriteAddr = mib_cast(4);

Void drv_std_read(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	if (Size > kib_cast(64))
		return;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	rt_copy_memory((VoidPtr)Buf, (VoidPtr)kReadAddr, Size);

	drv_std_select(IO);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + SectorSz - 1) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	Kernel::Detail::PRDEntry* prd = (Kernel::Detail::PRDEntry*)(kATADevice.Bar(0x20) + 4); // The PRDEntry is not correct.

	prd->mAddress	= (UInt32)(UIntPtr)kReadAddr;
	prd->mByteCount = Size - 1;
	prd->mFlags		= 0x8000; // indicate the end of prd.

	rt_out32(kATADevice.Bar(0x20) + 0x04, (UInt32)(UIntPtr)prd);

	rt_out8(kATADevice.Bar(0x20) + ATA_REG_COMMAND, ATA_CMD_READ_DMA);

	rt_out8(kATADevice.Bar(0x20) + 0x00, 0x09); // Start DMA engine

	while (rt_in8(kATADevice.Bar(0x20) + ATA_REG_STATUS) & 0x01)
		;

	rt_out8(kATADevice.Bar(0x20) + 0x00, 0x00); // Stop DMA engine

	rt_copy_memory((VoidPtr)kReadAddr, (VoidPtr)Buf, Size);

	delete prd;
	prd = nullptr;
}

Void drv_std_write(UInt64 Lba, UInt16 IO, UInt8 Master, Char* Buf, SizeT SectorSz, SizeT Size)
{
	Lba /= SectorSz;

	if (Size > kib_cast(64))
		return;

	UInt8 Command = ((!Master) ? 0xE0 : 0xF0);

	rt_copy_memory((VoidPtr)Buf, (VoidPtr)kWriteAddr, Size);

	rt_out8(IO + ATA_REG_HDDEVSEL, (Command) | (((Lba) >> 24) & 0x0F));

	rt_out8(IO + ATA_REG_SEC_COUNT0, ((Size + (SectorSz - 1)) / SectorSz));

	rt_out8(IO + ATA_REG_LBA0, (Lba)&0xFF);
	rt_out8(IO + ATA_REG_LBA1, (Lba) >> 8);
	rt_out8(IO + ATA_REG_LBA2, (Lba) >> 16);
	rt_out8(IO + ATA_REG_LBA3, (Lba) >> 24);

	Kernel::Detail::PRDEntry* prd = (Kernel::Detail::PRDEntry*)(kATADevice.Bar(0x20) + 4);

	prd->mAddress	= (UInt32)(UIntPtr)kWriteAddr;
	prd->mByteCount = Size - 1;
	prd->mFlags		= 0x8000;

	rt_out32(kATADevice.Bar(0x20) + 0x04, (UInt32)(UIntPtr)prd);
	rt_out8(kATADevice.Bar(0x20) + ATA_REG_COMMAND, ATA_CMD_WRITE_DMA);

	rt_out8(IO + 0x00, 0x09); // Start DMA engine

	while (rt_in8(kATADevice.Bar(0x20) + ATA_REG_STATUS) & 0x01)
		;

	rt_out8(kATADevice.Bar(0x20) + 0x00, 0x00); // Stop DMA engine

	delete prd;
	prd = nullptr;
}

/***********************************************************************************/
/// @brief Is ATA detected?
/***********************************************************************************/
Boolean drv_std_detected(Void)
{
	return kATADetected;
}

/***********************************************************************************/
/***
	@brief Gets the number of sectors inside the drive.
	@return Number of sectors, or zero.
*/
/***********************************************************************************/
Kernel::SizeT drv_get_sector_count()
{
	return (kATAData[61] << 16) | kATAData[60];
}

/***********************************************************************************/
/// @brief Get the size of the current drive.
/***********************************************************************************/
Kernel::SizeT drv_get_size()
{
	return (drv_get_sector_count()) * kATASectorSize;
}

#endif /* ifdef __ATA_DMA__ */
