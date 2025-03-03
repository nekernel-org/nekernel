/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef NE_USE_MBCI_FLASH

#include <NewKit/Defines.h>
#include <ArchKit/ArchKit.h>
#include <Mod/MFlash/MFlash.h>
#include <Mod/MBCI/MBCI.h>

/// @file MFlash.cc
/// @brief MBCI Flash support.

#define kMaxFlashSlots (8U)

namespace NeOS
{
	/// /Mount/Flash/n
	constexpr auto kFlashBridgeMagic	= 0x70768372;
	constexpr auto kFlashBridgeRevision = 1;

	STATIC BOOL	 kFlashEnabled							= NO;
	STATIC SizeT kFlashSize[kMaxFlashSlots]				= {};
	STATIC SizeT kFlashSectorSz[kMaxFlashSlots]			= {};
	STATIC IMBCIHost* kFlashMetaPackets[kMaxFlashSlots] = {};
	STATIC IMBCIHost* kFlashDataPackets[kMaxFlashSlots] = {};

	STATIC Void drv_std_io(Int32 slot, UInt64 lba, Char* buf, SizeT sector_sz, SizeT buf_sz);

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get slot sector count.
	/// @return slot sector count.
	SizeT drv_get_sector_count(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get slot full size (in bytes).
	/// @return drive slot size
	SizeT drv_get_size(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return 0;

		return kFlashSize[slot];
	}

	/// @brief Enable flash memory at slot.
	BOOL drv_enable_at(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return NO;

		kFlashMetaPackets[slot]->InterruptEnable = YES;

		kout << "Enabled hardware slot at: " << number(slot) << endl;

		return YES;
	}

	/// @brief Disable flash memory at slot.
	BOOL drv_disable_at(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return NO;

		kFlashMetaPackets[slot]->InterruptEnable = NO;

		kout << "Disabled hardware slot at: " << number(slot) << endl;

		return YES;
	}

	STATIC Void drv_std_io(Int32 slot, UInt64 lba, Char* buf, SizeT sector_sz, SizeT buf_sz)
	{
		UInt64* packet_frame = (UInt64*)kFlashDataPackets[slot]->BaseAddressRegister;

		if (packet_frame[0] != (UInt64)kFlashBridgeMagic)
			return;

		if (packet_frame[8] != (UInt64)kFlashBridgeRevision)
			return;

		packet_frame[16 + 0]  = lba;
		packet_frame[16 + 4]  = sector_sz;
		packet_frame[16 + 8]  = lba;
		packet_frame[16 + 12] = buf_sz;
		packet_frame[16 + 14] = (UIntPtr)HAL::hal_get_phys_address(buf);

		while (packet_frame[0] == lba)
			;
	}

	Void drv_std_read(Int32 slot, UInt64 lba, Char* buf, SizeT sector_sz, SizeT buf_sz)
	{
		rt_set_memory(buf, 0, buf_sz);

		drv_std_io(slot, lba, buf, sector_sz, buf_sz);
	}

	Void drv_std_write(Int32 slot, UInt64 lba, Char* buf, SizeT sector_sz, SizeT buf_sz)
	{
		drv_std_io(slot, lba, buf, sector_sz, buf_sz);
	}

} // namespace NeOS

#endif // if NE_USE_MBCI_FLASH
