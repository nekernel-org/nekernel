/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <user/SystemCalls.h>

/// @file SystemCalls.cc
/// @brief Source file for the memory functions of the libsci.

IMPORT_C VoidPtr sci_syscall_arg_1(SizeT id);
IMPORT_C VoidPtr sci_syscall_arg_2(SizeT id, VoidPtr arg1);
IMPORT_C VoidPtr sci_syscall_arg_3(SizeT id, VoidPtr arg1, VoidPtr arg3);
IMPORT_C VoidPtr sci_syscall_arg_4(SizeT id, VoidPtr arg1, VoidPtr arg3, VoidPtr arg4);

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

IMPORT_C SInt64 MmStrLen(const Char* in)
{
	if (!in)
		return 0;

	SizeT len{0};

	do
	{
		++len;
	} while (in[len] != '\0');

	return len;
}

/// @brief Fill memory region **dest** with **value**.
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

IMPORT_C Ref IoOpenFile(_Input const Char* path, _Input const Char* drv_letter)
{
	return sci_syscall_arg_3(1, reinterpret_cast<VoidPtr>(const_cast<Char*>(path)),
							 reinterpret_cast<VoidPtr>(const_cast<Char*>(drv_letter)));
}

IMPORT_C Void IoCloseFile(_Input Ref desc)
{
	sci_syscall_arg_2(2, desc);
}

IMPORT_C UInt64 IoSeekFile(_Input Ref desc, _Input UInt64 off)
{
	auto ret = (UInt64*)sci_syscall_arg_3(3, reinterpret_cast<VoidPtr>(desc),
										  reinterpret_cast<VoidPtr>(&off));

	MUST_PASS((*ret) != ~0UL);
	return *ret;
}

IMPORT_C UInt64 IoTellFile(_Input Ref desc)
{
	auto ret = (UInt64*)sci_syscall_arg_2(4, reinterpret_cast<VoidPtr>(desc));
	return *ret;
}
