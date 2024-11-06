/* -------------------------------------------

Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved..

File: LTE\LTE.h.
Purpose: LTE Standard Library.

------------------------------------------- */

#ifndef _INC_NETWORK_LTE_H_
#define _INC_NETWORK_LTE_H_

#include <NewKit/Defines.h>
#include <NewKit/KString.h>

/// @brief Long Term Evolution I/O routines.

/// @brief Turn on SIM slot.
Kernel::Boolean lte_turn_on_sim(Kernel::Int32 simSlot);

/// @brief Turn off SIM slot.
Kernel::Boolean lte_turn_off_sim(Kernel::Int32 simSlot);

/// @brief Send AT command.
Kernel::Boolean lte_send_at_command(Kernel::Char* buf,
									Kernel::Size  bufReadSz,
									Kernel::Int32 simSlot);

Kernel::Boolean lte_write_sim_file(Kernel::Char*   file,
								   Kernel::VoidPtr buf,
								   Kernel::Size	   bufSz,
								   Kernel::Size	   offset,
								   Kernel::Int32   simSlot);

Kernel::VoidPtr lte_read_sim_file(Kernel::Char* file,
								  Kernel::Size	bufSz,
								  Kernel::Size	offset,
								  Kernel::Int32 simSlot);

#endif // ifndef _INC_NETWORK_LTE_H_
