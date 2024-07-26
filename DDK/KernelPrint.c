/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <DDK/KernelPrint.h>

DK_EXTERN void kernelPrintChar(const char ch)
{
	char assembled[2] = {0};
	assembled[0]	  = ch;
	assembled[1]	  = 0;

	kernelCall("WriteCharacter", 1, assembled, 1);
}

/// @brief print string to UART.
/// @param message UART to transmit.
DK_EXTERN void kernelPrintStr(const char* message)
{
	if (!message)
		return;
	if (*message == 0)
		return;

	size_t index = 0;
	size_t len	 = kernelStringLength(message);

	while (index < len)
	{
		kernelPrintChar(message[index]);
		++index;
	}
}
