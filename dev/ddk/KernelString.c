/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: DDK Strings.

------------------------------------------- */

#include <ddk/KernelString.h>

DK_EXTERN size_t KernelStringLength(const char* in)
{
	if (in == nil)
		return 0;
	if (*in == 0)
		return 0;

	size_t index = 0;

	while (in[index] != 0)
	{
		++index;
	}

	return index;
}

DK_EXTERN int KernelStringCopy(char* dst, const char* src, size_t len)
{
	size_t index = 0;

	while (index != len)
	{
		dst[index] = src[index];
		++index;
	}

	return index;
}
