/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: IO-Impl-AMD64.hpp
	Purpose: I/O for AMD64.

	Revision History:

	30/01/24: Add file. (amlel)
	02/02/24: Update I/O routines. (amlel)

------------------------------------------- */

namespace NewOS
{
	template <SizeT Sz>
	template <typename T>
	T IOArray<Sz>::In(SizeT index)
	{
		switch (sizeof(T))
		{
		case 4:
			return HAL::In32(fPorts[index].Leak());
		case 2:
			return HAL::In16(fPorts[index].Leak());
		case 1:
			return HAL::In8(fPorts[index].Leak());
		default:
			return 0xFFFF;
		}
	}

	template <SizeT Sz>
	template <typename T>
	void IOArray<Sz>::Out(SizeT index, T value)
	{
		switch (sizeof(T))
		{
#ifdef __x86_64__
		case 4:
			HAL::Out32(fPorts[index].Leak(), value);
		case 2:
			HAL::Out16(fPorts[index].Leak(), value);
		case 1:
			HAL::Out8(fPorts[index].Leak(), value);
#endif
		default:
			break;
		}
	}
} // namespace NewOS
