/* -------------------------------------------

	Copyright (C) 2024, ELMH GROUP, all rights reserved.

------------------------------------------- */

#include <SCIKit/SCI.h>

/// @file SCI.cc
/// @brief Source file for the memory functions of the SCI.

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
