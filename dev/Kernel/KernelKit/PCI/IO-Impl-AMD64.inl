/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

	File: IO-Impl-AMD64.h
	Purpose: I/O for AMD64.

	Revision History:

	30/01/24: Add file. (amlel)
	02/02/24: Update I/O routines. (amlel)

------------------------------------------- */

namespace Kernel
{
	template <SizeT Sz>
	template <typename T>
	T IOArray<Sz>::In(SizeT index)
	{
		switch (sizeof(T))
		{
#ifdef __ZKA_AMD64__
		case 4:
			return HAL::rt_in32(fPorts[index].Leak());
		case 2:
			return HAL::rt_in16(fPorts[index].Leak());
		case 1:
			return HAL::rt_in8(fPorts[index].Leak());
#endif
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
#ifdef __ZKA_AMD64__
		case 4:
			HAL::rt_out32(fPorts[index].Leak(), value);
		case 2:
			HAL::rt_out16(fPorts[index].Leak(), value);
		case 1:
			HAL::rt_out8(fPorts[index].Leak(), value);
#endif
		default:
			break;
		}
	}
} // namespace Kernel
