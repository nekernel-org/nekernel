/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Strings.

------------------------------------------- */

#include <DDK/KernelString.h>

DK_EXTERN size_t kernelStringLength(const char* in)
{
	if (in == nil) return 0;
	if (*in == 0) return 0;

	size_t index = 0;

	while (in[index] != 0)
	{
		++index;
	}

	return index;
}

DK_EXTERN int kernelStringCopy(char* dst, const char* src, size_t len)
{
	size_t index = 0;

	while (index != len)
	{
		dst[index] = src[index];
		++index;
	}

	return index;
}
