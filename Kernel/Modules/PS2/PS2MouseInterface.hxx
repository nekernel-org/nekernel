/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: PS2MouseInterface.hxx
	Purpose: PS/2 mouse.

	Revision History:

	03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

namespace Kernel
{
	/// @brief PS/2 Mouse driver interface
	class PS2MouseInterface final
	{
	public:
		explicit PS2MouseInterface() = default;
		~PS2MouseInterface()		 = default;

		NEWOS_COPY_DEFAULT(PS2MouseInterface);

	public:
		/// @brief Enables PS2 mouse for kernel.
		/// @return
		Void Init() noexcept
		{
			HAL::rt_cli();

			HAL::Out8(0x64, 0xA8); // enabling the auxiliary device - mouse

			this->Wait();
			HAL::Out8(0x64, 0x20); // tells the keyboard controller that we want to send a command to the mouse
			this->WaitInput();

			UInt8 status = HAL::In8(0x60);
			status |= 0b10;

			this->Wait();
			HAL::Out8(0x64, 0x60);
			this->Wait();
			HAL::Out8(0x60, status); // setting the correct bit is the "compaq" status byte

			this->Write(0xF6);
			this->Read();

			this->Write(0xF4);
			this->Read();

			HAL::rt_sti();
		}

	public:
		Bool WaitInput() noexcept
		{
			UInt64 timeout = 100000;

			while (timeout)
			{
				if ((HAL::In8(0x64) & 0x1))
				{
					return true;
				}

				--timeout;
			} // wait until we can read

			// return the ack bit.
			return false;
		}

		Bool Wait() noexcept
		{
			UInt64 timeout = 100000;

			while (timeout)
			{
				if ((HAL::In8(0x64) & 0b10) == 0)
				{
					return true;
				}

				--timeout;
			} // wait until we can read

			// return the ack bit.
			return false;
		}

		Void Write(UInt8 val)
		{
			HAL::Out8(0x64, 0xD4);
			this->Wait();
			HAL::Out8(0x60, val);
			this->Wait();
		}

		UInt8 Read()
		{
			this->WaitInput();
			return HAL::In8(0x60);
		}
	};
} // namespace Kernel
