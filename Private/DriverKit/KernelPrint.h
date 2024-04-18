/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: Kernel Text I/O.

------------------------------------------- */

#pragma once

#if defined(__cplusplus)
#define DK_EXTERN extern "C"
#else
#define DK_EXTERN extern
#endif // defined(__cplusplus)

#include <stdint.h>
#include <stddef.h>

/// @brief print character into UART.
DK_EXTERN void kernelPrintChar(const char ch);

static inline size_t kernelStringLength(const char* str) {
    size_t index = 0;

    while (str[index] != 0) {
        ++index;
    }

    return index;
}

/// @brief print string to UART.
static inline void kernelPrintStr(const char* message) {
    if (!message) return;
    if (*message == 0) return;

    size_t index = 0;
    size_t len = kernelStringLength(message);

    while (index < len) {
      kernelPrintChar(message[index]);
      ++index;
    }
}
