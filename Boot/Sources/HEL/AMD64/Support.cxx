/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <BootKit/Support.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>

#ifdef __STANDALONE__

EXTERN_C void __cxa_pure_virtual()
{
	BTextWriter writer;
	writer.Write("newosldr: Placeholder.\r");
}

/// @brief memset definition in C++.
/// @param dst destination pointer.
/// @param byte value to fill in.
/// @param len length of of src.
EXTERN_C VoidPtr memset(void* dst, int byte, long long unsigned int len)
{
	for (size_t i = 0UL; i < len; ++i)
	{
		((int*)dst)[i] = byte;
	}

	return dst;
}

/// @brief memcpy definition in C++.
/// @param dst destination pointer.
/// @param  src source pointer.
/// @param len length of of src.
EXTERN_C VoidPtr memcpy(void* dst, const void* src, long long unsigned int len)
{
	for (size_t i = 0UL; i < len; ++i)
	{
		((int*)dst)[i] = ((int*)src)[i];
	}

	return dst;
}

/// @brief strlen definition in C++.
EXTERN_C size_t strlen(const char* whatToCheck)
{
	if (!whatToCheck || *whatToCheck == 0)
		return 0;

	SizeT len = 0;

	while (whatToCheck[len] != 0)
	{
		++len;
	}

	return len;
}

/// @brief somthing specific to the Microsoft's ABI, When the stack grows too big.
EXTERN_C void ___chkstk_ms(void)
{
}

#endif
