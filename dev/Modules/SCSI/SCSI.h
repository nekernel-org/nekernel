/* -------------------------------------------

	Copyright (C) 2024, TQ B.V, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file SCSI.h
/// @brief Serial SCSI driver.

template <int PacketBitLen>
using scsi_packet_type = Kernel::UInt16[PacketBitLen];
