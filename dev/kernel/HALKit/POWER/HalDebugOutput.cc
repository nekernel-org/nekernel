/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/POWER/Processor.h>
#include <KernelKit/DebugOutput.h>

using namespace NeOS;

/// @brief Writes to COM1.
/// @param bytes
void ke_io_write(const Char* bytes)
{
	if (!bytes)
		return;

	SizeT index = 0;
	SizeT len	= rt_string_len(bytes, 256U);

	while (index < len)
	{
		// TODO
		++index;
	}
}
