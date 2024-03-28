/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/PCI/Iterator.hpp>

#define PCI_ITERATOR_FIND_AND_UNWRAP(DEV, SZ) \
  if (DEV.Leak()) return DEV.Leak();

namespace NewOS::PCI {
Iterator::Iterator(const Types::PciDeviceKind &type) {
  // probe devices.
  for (int bus = 0; bus < ME_BUS_COUNT; ++bus) {
    for (int device = 0; device < ME_DEVICE_COUNT; ++device) {
      for (int function = 0; function < ME_FUNCTION_COUNT; ++function) {
        Device dev(bus, device, function, 0);

        if (dev.Class() == (UChar)type) {
          m_Devices[bus].Leak().Leak() = dev;
        }
      }
    }
  }
}

Iterator::~Iterator() {}

Ref<PCI::Device> Iterator::operator[](const Size &sz) {
  PCI_ITERATOR_FIND_AND_UNWRAP(m_Devices[sz], sz);
  return {};
}
}  // namespace NewOS::PCI
