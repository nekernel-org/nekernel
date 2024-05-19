/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace NewOS
{
	template <SizeT Sz>
	class IOArray final
	{
	public:
		IOArray() = delete;

		IOArray(nullPtr) = delete;

		explicit IOArray(Array<UShort, Sz>& ports)
			: fPorts(ports)
		{
		}
		~IOArray()
		{
		}

		IOArray& operator=(const IOArray&) = default;

		IOArray(const IOArray&) = default;

		operator bool()
		{
			return !fPorts.Empty();
		}

	public:
		template <typename T>
		T In(SizeT index);

		template <typename T>
		void Out(SizeT index, T value);

	private:
		Array<UShort, Sz> fPorts;
	};

	using IOArray16 = IOArray<16>;
} // namespace NewOS

#ifdef __x86_64__
#include <KernelKit/PCI/IO-Impl-AMD64.inl>
#else
#error Please provide platform specific code for the I/O
#endif // ifdef __x86_64__
