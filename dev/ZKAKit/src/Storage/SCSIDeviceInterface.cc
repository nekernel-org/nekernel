/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

------------------------------------------- */

#include <StorageKit/SCSI.h>

///! @brief ATAPI SCSI packet.
const scsi_packet_type<12> kCDRomPacketTemplate = {0x43, 0, 1, 0, 0, 0,
												   0, 12, 0x40, 0, 0};
