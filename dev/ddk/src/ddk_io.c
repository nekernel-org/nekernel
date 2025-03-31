/* -------------------------------------------

	Copyright Amlal El Mahrouss.

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <DDKKit/io.h>

DDK_EXTERN void kputc(const char ch)
{
	char assembled[2] = {0};
	assembled[0]	  = ch;
	assembled[1]	  = 0;

	ke_call("ke_put_string", 1, assembled, 1);
}

/// @brief print string to UART.
/// @param message UART to transmit.
DDK_EXTERN void kprint(const char* message)
{
	if (!message)
		return;
	if (*message == 0)
		return;

	size_t index = 0;
	size_t len	 = kstrlen(message);

	while (index < len)
	{
		kputc(message[index]);
		++index;
	}
}
