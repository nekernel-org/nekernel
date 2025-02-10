/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <CompilerKit/CompilerKit.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/Stream.h>

#define kDebugMaxPorts 56

#define kDebugUnboundPort 0x0FEED

#define kDebugMag0 'Z'
#define kDebugMag1 'D'
#define kDebugMag2 'B'
#define kDebugMag3 'G'

#define kDebugSourceFile 0
#define kDebugLine		 33
#define kDebugTeam		 43
#define kDebugEOP		 49

namespace Kernel
{
	class TerminalDevice;
	class DTraceDevice;

	inline TerminalDevice end_line();
	inline TerminalDevice number(const Long& x);
	inline TerminalDevice hex_number(const Long& x);

	// @brief Emulates a VT100 terminal.
	class TerminalDevice final NE_DEVICE<const Char*>
	{
	public:
		TerminalDevice(void (*print)(const Char*), void (*gets)(const Char*))
			: IDeviceObject<const Char*>(print, gets)
		{
		}

		~TerminalDevice() override;

		/// @brief returns device name (terminal name)
		/// @return string type (const Char*)
		const Char* Name() const override
		{
			return ("TerminalDevice");
		}

		NE_COPY_DEFAULT(TerminalDevice);

		STATIC TerminalDevice The() noexcept;
	};

	inline TerminalDevice end_line()
	{
		TerminalDevice self = TerminalDevice::The();

		self.operator<<("\r");
		return self;
	}

	inline TerminalDevice carriage_return()
	{
		TerminalDevice self = TerminalDevice::The();

		self.operator<<("\r");
		return self;
	}

	inline TerminalDevice tabulate()
	{
		TerminalDevice self = TerminalDevice::The();

		self.operator<<("\t");
		return self;
	}

	/// @brief emulate a terminal bell, like the VT100 does.
	inline TerminalDevice bell()
	{
		TerminalDevice self = TerminalDevice::The();

		self.operator<<("\a");
		return self;
	}

	namespace Detail
	{
		inline TerminalDevice _write_number(const Long& x, TerminalDevice& term)
		{
			UInt64 y = (x > 0 ? x : -x) / 10;
			UInt64 h = (x > 0 ? x : -x) % 10;

			if (y)
				_write_number(y, term);

			/* fail if the number is not base-10 */
			if (h > 10)
			{
				_write_number('?', term);
				return term;
			}

			if (y < 0)
				y = -y;

			const Char kNumbers[11] = "0123456789";

			Char buf[2];
			buf[0] = kNumbers[h];
			buf[1] = 0;

			term.operator<<(buf);
			return term;
		}

		inline TerminalDevice _write_number_hex(const Long& x, TerminalDevice& term)
		{
			UInt64 y = (x > 0 ? x : -x) / 16;
			UInt64 h = (x > 0 ? x : -x) % 16;

			if (y)
				_write_number_hex(y, term);

			/* fail if the hex number is not base-16 */
			if (h > 16)
			{
				_write_number_hex('?', term);
				return term;
			}

			if (y < 0)
				y = -y;

			const Char kNumbers[17] = "0123456789ABCDEF";

			Char buf[2];
			buf[0] = kNumbers[h];
			buf[1] = 0;

			term.operator<<(buf);
			return term;
		}
	} // namespace Detail

	inline TerminalDevice hex_number(const Long& x)
	{
		TerminalDevice self = TerminalDevice::The();

		Detail::_write_number_hex(x, self);
		self.operator<<("h");

		return self;
	}

	inline TerminalDevice number(const Char* x)
	{
		TerminalDevice self = TerminalDevice::The();

		self << "?";

		return self;
	}

	inline TerminalDevice number(const Long& x)
	{
		TerminalDevice self = TerminalDevice::The();

		Detail::_write_number(x, self);

		return self;
	}

	inline TerminalDevice get_console_in(Char* buf)
	{
		TerminalDevice self = TerminalDevice::The();

		self >> buf;

		return self;
	}

	typedef Char rt_debug_type[255];

	class DebuggerPortHeader final
	{
	public:
		Int16 fPort[kDebugMaxPorts];
		Int16 fBoundCnt;
	};

	inline TerminalDevice& operator<<(TerminalDevice& src, auto number)
	{
		number(number, src);
		return src;
	}
} // namespace Kernel

#ifdef kout
#undef kout
#endif // ifdef kout

#define kout Kernel::TerminalDevice::The() << "[NeKernel] *" << __FILE__ << "*: "

#define endl Kernel::TerminalDevice::The() << Kernel::end_line()
