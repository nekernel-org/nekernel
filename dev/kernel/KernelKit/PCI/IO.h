/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

namespace NeOS
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

	inline constexpr UInt16 kMaxPorts = 16;

	using IOArray16 = IOArray<kMaxPorts>;

	template <SizeT Sz>
	inline Array<UShort, Sz> make_ports(UShort base)
	{
		Array<UShort, Sz> ports;

		for (UShort i = 0; i < Sz; ++i)
		{
			ports[i] = base + i;
		}

		return ports;
	}
} // namespace NeOS

#ifdef __x86_64__
#include <KernelKit/PCI/IOArray+AMD64.inl>
#else
#error Please provide platform specific code for the I/O
#endif // ifdef __x86_64__
