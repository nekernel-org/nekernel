/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved..

File: LTE\LTE.h.
Purpose: LTE Standard Library.

------------------------------------------- */

#ifndef _INC_NETWORK_LTE_H_
#define _INC_NETWORK_LTE_H_

#include <NewKit/Defines.h>
#include <NewKit/KString.h>

/// @brief Long Term Evolution I/O routines.

/// @brief Turn on SIM slot.
NeOS::Boolean lte_turn_on_sim(NeOS::Int32 simSlot);

/// @brief Turn off SIM slot.
NeOS::Boolean lte_turn_off_sim(NeOS::Int32 simSlot);

/// @brief Send AT command.
NeOS::Boolean lte_send_at_command(NeOS::Char* buf,
								  NeOS::Size  bufReadSz,
								  NeOS::Int32 simSlot);

NeOS::Boolean lte_write_sim_file(NeOS::Char*   file,
								 NeOS::VoidPtr buf,
								 NeOS::Size	   bufSz,
								 NeOS::Size	   offset,
								 NeOS::Int32   simSlot);

NeOS::VoidPtr lte_read_sim_file(NeOS::Char* file,
								NeOS::Size	bufSz,
								NeOS::Size	offset,
								NeOS::Int32 simSlot);

#endif // ifndef _INC_NETWORK_LTE_H_
