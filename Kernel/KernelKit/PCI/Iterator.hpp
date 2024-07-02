#ifndef __PCI_ITERATOR_HPP__
#define __PCI_ITERATOR_HPP__

#include <KernelKit/PCI/Database.hpp>
#include <KernelKit/PCI/Device.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

#define NEWOS_BUS_COUNT		 (256)
#define NEWOS_DEVICE_COUNT	 (33)
#define NEWOS_FUNCTION_COUNT (8)

namespace Kernel::PCI
{
	class Iterator final
	{
	public:
		Iterator() = delete;

	public:
		explicit Iterator(const Types::PciDeviceKind& deviceType);

		Iterator& operator=(const Iterator&) = default;

		Iterator(const Iterator&) = default;

		~Iterator();

	public:
		Ref<PCI::Device> operator[](const Size& sz);

	private:
		Array<PCI::Device, NEWOS_BUS_COUNT> fDevices;
	};
} // namespace Kernel::PCI

#endif // __PCI_ITERATOR_HPP__
