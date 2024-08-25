/* -------------------------------------------

	Copyright ZKA Technologies.

	File: PS2MouseInterface.hxx
	Purpose: PS/2 mouse.

	Revision History:

	03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hxx>

namespace Kernel
{
	/// @brief PS/2 Mouse driver interface
	class PS2MouseInterface final
	{
	public:
		explicit PS2MouseInterface() = default;
		~PS2MouseInterface()		 = default;

		ZKA_COPY_DEFAULT(PS2MouseInterface);

	public:
		/// @brief Enables PS2 mouse for kernel.
		/// @return
		Void Init() noexcept
		{
			HAL::Out8(0x64, 0xA8); // enabling the auxiliary device - mouse

			this->Wait();

			HAL::Out8(0x64, 0x20); // tells the keyboard controller that we want to send a command to the mouse

			this->Wait();

			UInt8 status = HAL::In8(0x60);
			status |= 0x02;

			HAL::Out8(0x64, 0x60);
			HAL::Out8(0x60, status); // setting the correct bit is the "compaq" status byte

			HAL::Out8(0x64, 0xD4);
			HAL::Out8(0x60, 0xFA); // setting the correct bit is the "compaq" status byte

			this->Read();

			// Unmask mouse IRQ.

			// get slave PIC.
			UInt8 old_pic = HAL::In8(0x21);

			// enable mosue interrupts
			HAL::Out8(0x21, old_pic & (~0b00000100));
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
