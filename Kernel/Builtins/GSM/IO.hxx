/* -------------------------------------------

Copyright Zeta Electronics Corporation.

File: GSM\IO.hxx.
Purpose: GSM I/O.

------------------------------------------- */

#ifndef _INC_NETWORK_GSM_IO_HXX_
#define _INC_NETWORK_GSM_IO_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief GSM I/O routines.

/// @brief Turn on SIM slot.
NewOS::Boolean gsm_turn_on_slot(NewOS::Int32 slot);

/// @brief Turn off SIM slot.
NewOS::Boolean gsm_turn_off_slot(NewOS::Int32 slot);

/// @brief Send AT command.
NewOS::Boolean gsm_send_at_command(NewOS::Char* buf,
								   NewOS::Size bufSz);


#endif // ifndef _INC_NETWORK_GSM_IO_HXX_
