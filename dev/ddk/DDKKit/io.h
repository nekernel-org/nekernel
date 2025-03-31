/* -------------------------------------------

	Copyright Amlal El Mahrouss.

	Purpose: DDK Text I/O.

------------------------------------------- */

#pragma once

#include <DDKKit/str.h>

/// @brief print character into UART.
DDK_EXTERN void kputc(const char ch);

/// @brief print string to UART.
/// @param message string to transmit to UART.
DDK_EXTERN void kprint(const char* message);
