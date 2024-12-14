/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <NewKit/Defines.h>

namespace Kernel::PCI
{
	enum class PciConfigKind : UShort
	{
		ConfigAddress = 0xCF8,
		ConfigData	  = 0xCFC,
		Invalid		  = 0xFFF
	};

	class Device final
	{
	public:
		Device() = default;

	public:
		explicit Device(UShort bus, UShort device, UShort function, UInt32 bar);

		Device& operator=(const Device&) = default;

		Device(const Device&) = default;

		~Device();

	public:
		UInt Read(UInt bar, Size szData);
		void Write(UInt bar, UIntPtr data, Size szData);

	public:
		operator bool();

	public:
		template <typename T>
		UInt Read(UInt bar)
		{
			static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
			return Read(bar, sizeof(T));
		}

		template <typename T>
		void Write(UInt bar, UIntPtr data)
		{
			static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
			Write(bar, data, sizeof(T));
		}

	public:
		UShort	DeviceId();
		UShort	VendorId();
		UShort	InterfaceId();
		UChar	Class();
		UChar	Subclass();
		UChar	ProgIf();
		UChar	HeaderType();
		UIntPtr Bar(UInt32 bar_in);

	public:
		void EnableMmio(UInt32 bar_in);
		void BecomeBusMaster(UInt32 bar_in); // for PCI-DMA, PC-DMA does not need that.

		UShort Vendor();

	private:
		UShort fBus;
		UShort fDevice;
		UShort fFunction;
		UInt32 fBar;
	};
} // namespace Kernel::PCI

EXTERN_C void NewOSPCISetCfgTarget(Kernel::UInt bar);
EXTERN_C Kernel::UInt NewOSPCIReadRaw(Kernel::UInt bar);
