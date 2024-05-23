/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

namespace NewOS
{
	template <class T>
	T* DMAWrapper::operator->()
	{
		return fAddress;
	}

	template <class T>
	T* DMAWrapper::Get(const UIntPtr offset)
	{
		return reinterpret_cast<T*>((UIntPtr)fAddress + offset);
	}
} // namespace NewOS
