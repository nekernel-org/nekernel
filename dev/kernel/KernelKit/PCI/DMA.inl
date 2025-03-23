/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

namespace NeOS
{
	template <class T>
	T* DMAWrapper::operator->()
	{
		return this->fAddress;
	}

	template <class T>
	T* DMAWrapper::Get(const UIntPtr offset)
	{
		return reinterpret_cast<T*>((UIntPtr)this->fAddress + offset);
	}
} // namespace NeOS
