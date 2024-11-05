/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <CompilerKit/CompilerKit.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/Stream.h>

#define kDebugMaxPorts 16

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

	inline TerminalDevice end_line();
	inline TerminalDevice number(const Long& x);
	inline TerminalDevice hex_number(const Long& x);

	// @brief Emulates a VT100 terminal.
	class TerminalDevice final : public DeviceInterface<const Char*>
	{
	public:
		TerminalDevice(void (*print)(const Char*), void (*get)(const Char*))
			: DeviceInterface<const Char*>(print, get)
		{
		}

		virtual ~TerminalDevice() = default;

		/// @brief returns device name (terminal name)
		/// @return string type (const Char*)
		virtual const Char* Name() const override
		{
			return ("TerminalDevice");
		}

		ZKA_COPY_DEFAULT(TerminalDevice);

		STATIC TerminalDevice The() noexcept;
	};

	inline TerminalDevice end_line()
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		selfTerm.operator<<("\r");
		return selfTerm;
	}

	inline TerminalDevice carriage_return()
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		selfTerm.operator<<("\r");
		return selfTerm;
	}

	inline TerminalDevice tabulate()
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		selfTerm.operator<<("\t");
		return selfTerm;
	}

	/// @brief emulate a terminal bell, like the VT100 does.
	inline TerminalDevice bell()
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		selfTerm.operator<<("\a");
		return selfTerm;
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
		TerminalDevice selfTerm = TerminalDevice::The();

		Detail::_write_number_hex(x, selfTerm);
		selfTerm.operator<<("h");

		return selfTerm;
	}

	inline TerminalDevice number(const Long& x)
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		Detail::_write_number(x, selfTerm);

		return selfTerm;
	}

	inline TerminalDevice get_console_in(Char* buf)
	{
		TerminalDevice selfTerm = TerminalDevice::The();

		selfTerm >> buf;

		return selfTerm;
	}

	typedef Char rt_debug_type[255];

	class DebuggerPortHeader final
	{
	public:
		Int16 fPort[kDebugMaxPorts];
		Int16 fBoundCnt;
	};
} // namespace Kernel

#ifdef kcout
#undef kcout
#endif // ifdef kcout

#define kcout                                                                                               \
	(Kernel::TerminalDevice::The() << "\e[0;31m [ " << __FILE__ << ": LINE: " << Kernel::number(__LINE__)); \
	(Kernel::TerminalDevice::The() << " ] \e[0m"                                                            \
								   << ": ")
#define endl Kernel::TerminalDevice::The() << Kernel::end_line()
