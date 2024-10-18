/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

/// @file SCSI.hxx
/// @brief Serial SCSI driver.

template <int PacketBitLen>
using scsi_packet_type = Kernel::UInt16[PacketBitLen];
