/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <SCIKit/Foundation.h>

/// @file sci_base.cc
/// @brief Base Memory Manager functions for SCI.dll

/// @brief Copy memory region.
IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len)
{
	if (!len ||
		!dest ||
		!src)
	{
		return nullptr;
	}

	for (SizeT i = 0; i < len; i++)
	{
		((Char*)dest)[i] = ((Char*)src)[i];
	}

	return dest;
}

/// @brief Fill memory region with **value**.
IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value)
{
	if (!len ||
		!dest)
	{
		return nullptr;
	}

	for (SizeT i = 0; i < len; i++)
	{
		((Char*)dest)[i] = value;
	}

	return dest;
}
