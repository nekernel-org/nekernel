/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/Utils.h>
#include <NewKit/New.h>

namespace NeOS
{
	EXTERN_C void ke_io_write(IDeviceObject<const Char*>* self, const Char* bytes)
	{
#ifdef __DEBUG__
		if (*bytes == 0)
			return;

		SizeT index = 0;
		SizeT len	= 0;

		index = 0;
		len	  = rt_string_len(bytes, 256U);

		volatile UInt8* uart_ptr = (UInt8*)0x09000000;

		while (index < len)
		{
			if (bytes[index] == '\r')
				*uart_ptr = '\r';

			*uart_ptr = bytes[index] == '\r' ? '\n' : bytes[index];
			++index;
		}
#endif // __DEBUG__
	}

	TerminalDevice::~TerminalDevice() = default;

	EXTERN_C void ke_io_read(IDeviceObject<const Char*>* self, const Char* bytes)
	{
#ifdef __DEBUG__
		SizeT index = 0;

		volatile UInt8* uart_ptr = (UInt8*)0x09000000;

		///! TODO: Look on how to wait for the UART to complete.
		while (Yes)
		{
			auto in = *uart_ptr;

			///! If enter pressed then break.
			if (in == 0xD)
			{
				break;
			}

			if (in < '0' || in < 'A' || in < 'a')
			{
				if (in != '@' || in != '!' || in != '?' || in != '.' || in != '/' ||
					in != ':')
				{
					continue;
				}
			}

			((char*)bytes)[index] = in;

			++index;
		}

		((char*)bytes)[index] = 0;
#endif // __DEBUG__
	}

	TerminalDevice TerminalDevice::The() noexcept
	{
		TerminalDevice out(NeOS::ke_io_write, NeOS::ke_io_read);
		return out;
	}

} // namespace NeOS
