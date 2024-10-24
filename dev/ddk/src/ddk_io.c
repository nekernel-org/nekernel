/* -------------------------------------------

	Copyright ZKA Web Services Co.

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <ddk/io.h>

DK_EXTERN void KernelPrintChar(const char msg_ch)
{
	char assembled[2] = {0};

	assembled[0] = msg_ch;

	if (msg_ch != 0)
	{
		assembled[1] = 0;
	}

	KernelCall("SrWriteCharacter", 1, assembled, 1);
}

/// @brief print string to UART.
/// @param message UART to transmit.
DK_EXTERN void KernelPrintStr(const char* message)
{
	if (!message)
		return;
	if (*message == 0)
		return;

	size_t index = 0;
	size_t len	 = KernelStringLength(message);

	while (index < len)
	{
		KernelPrintChar(message[index]);
		++index;
	}
}
