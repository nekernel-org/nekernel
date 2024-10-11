/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <sci/sci_base.hxx>

/// @file sci_base.cxx
/// @brief Base Memory Manager functions for SCI.dll

/// @brief Debug error prompt, when a function misbehaves.
/// @param msg
/// @return
IMPORT_C Void __RtlRaiseSoftError(const char* msg);

/// @brief Copy memory region.
IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len)
{
	if (!len ||
		!dest ||
		!src)
	{
		__RtlRaiseSoftError("Debug Error, MmCopyMemory contains one or more invalid arguments.");
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
		__RtlRaiseSoftError("Debug Error, MmFillMemory contains one or more invalid arguments.");
		return nullptr;
	}

	for (SizeT i = 0; i < len; i++)
	{
		((Char*)dest)[i] = value;
	}

	return dest;
}
