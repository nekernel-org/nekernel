/* -------------------------------------------

	Copyright SoftwareLabs

	Purpose: Kernel Text I/O.

------------------------------------------- */

#pragma once

#include <DDK/KernelString.h>

/// @brief print character into UART.
DK_EXTERN void kernelPrintChar(const char ch);

/// @brief print string to UART.
/// @param message UART to transmit.
DK_EXTERN void kernelPrintStr(const char* message);
