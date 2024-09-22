/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __PCI_ITERATOR_HPP__
#define __PCI_ITERATOR_HPP__

#include <KernelKit/PCI/Database.hxx>
#include <KernelKit/PCI/Device.hxx>
#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Ref.hxx>

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

#endif // __PCI_ITERATOR_HPP__
