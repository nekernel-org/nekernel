/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: Kernel Strings.

------------------------------------------- */

#include <DriverKit/KernelString.h>

DK_EXTERN size_t kernelStringLength(const char* str) {
    size_t index = 0;

    while (str[index] != 0) {
        ++index;
    }

    return index;
}

DK_EXTERN int kernelStringCopy(char* dst, const char* src, size_t len) {
    size_t index = 0;

    while (index != len) {
        dst[index] = src[index];
        ++index;
    }

    return index;
}
