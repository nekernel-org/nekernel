/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __PCI_ITERATOR_H__
#define __PCI_ITERATOR_H__

#include <KernelKit/PCI/Database.h>
#include <KernelKit/PCI/Device.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

#define ZKA_BUS_COUNT	   (256)
#define ZKA_DEVICE_COUNT   (33)
#define ZKA_FUNCTION_COUNT (8)

namespace Kernel::PCI
{
	class Iterator final
	{
	public:
		Iterator() = delete;

	public:
		explicit Iterator(const Types::PciDeviceKind& deviceType);

		Iterator& operator=(const Iterator&) = default;
		Iterator(const Iterator&)			 = default;

		~Iterator();

	public:
		Ref<PCI::Device> operator[](const Size& sz);

	private:
		Array<PCI::Device, ZKA_BUS_COUNT> fDevices;
	};
} // namespace Kernel::PCI

#endif // __PCI_ITERATOR_H__
