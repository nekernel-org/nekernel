/* -------------------------------------------

Copyright ZKA Technologies.

File: LTE\IO.hxx.
Purpose: LTE I/O.

------------------------------------------- */

#ifndef _INC_NETWORK_LTE_IO_HXX_
#define _INC_NETWORK_LTE_IO_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief Long Term Evolution I/O routines.

/// @brief Turn on SIM slot.
Kernel::Boolean lte_turn_on_slot(Kernel::Int32 slot);

/// @brief Turn off SIM slot.
Kernel::Boolean lte_turn_off_slot(Kernel::Int32 slot);

/// @brief Send AT command.
Kernel::Boolean lte_send_at_command(Kernel::Char* buf,
									Kernel::Size  bufSz);

#endif // ifndef _INC_NETWORK_LTE_IO_HXX_
