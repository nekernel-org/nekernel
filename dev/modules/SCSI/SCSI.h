/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file SCSI.h
/// @brief Small Computer System Interface device.

namespace Kernel
{
	template <int PacketBitLen>
	using scsi_packet_type	  = Kernel::UInt16[PacketBitLen];
	using scsi_packet_type_12 = scsi_packet_type<12>;

	extern const scsi_packet_type<12> kCDRomPacketTemplate;
} // namespace Kernel