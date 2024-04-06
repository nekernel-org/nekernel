/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/PCI/Device.hpp>

NewOS::UInt NewOSPCIReadRaw(NewOS::UInt bar, NewOS::UShort bus,
                            NewOS::UShort dev, NewOS::UShort fun) {
  NewOS::UInt target = 0x80000000 | ((NewOS::UInt)bus << 16) |
                       ((NewOS::UInt)dev << 11) | ((NewOS::UInt)fun << 8) |
                       (bar & 0xFC);

  NewOS::HAL::Out32((NewOS::UShort)NewOS::PCI::PciConfigKind::ConfigAddress,
                    target);

  return NewOS::HAL::In32((NewOS::UShort)NewOS::PCI::PciConfigKind::ConfigData);
}

void NewOSPCISetCfgTarget(NewOS::UInt bar, NewOS::UShort bus, NewOS::UShort dev,
                          NewOS::UShort fun) {
  NewOS::UInt target = 0x80000000 | ((NewOS::UInt)bus << 16) |
                       ((NewOS::UInt)dev << 11) | ((NewOS::UInt)fun << 8) |
                       (bar & ~3);

  NewOS::HAL::Out32((NewOS::UShort)NewOS::PCI::PciConfigKind::ConfigAddress,
                    target);
}

namespace NewOS::PCI {
Device::Device(UShort bus, UShort device, UShort func, UShort bar)
    : fBus(bus), fDevice(device), fFunction(func), fBar(bar) {}

Device::~Device() {}

UInt Device::Read(UInt bar, Size sz) {
  NewOSPCISetCfgTarget(bar, fBus, fDevice, fFunction);

  if (sz == 4)
    return HAL::In32((UShort)PciConfigKind::ConfigData + (fBar & 3));
  if (sz == 2)
    return HAL::In16((UShort)PciConfigKind::ConfigData + (fBar & 3));
  if (sz == 1) return HAL::In8((UShort)PciConfigKind::ConfigData + (fBar & 3));

  return 0xFFFF;
}

void Device::Write(UInt bar, UIntPtr data, Size sz) {
  NewOSPCISetCfgTarget(bar, fBus, fDevice, fFunction);

  if (sz == 4)
    HAL::Out32((UShort)PciConfigKind::ConfigData + (fBar & 3), (UInt)data);
  if (sz == 2)
    HAL::Out16((UShort)PciConfigKind::ConfigData + (fBar & 3), (UShort)data);
  if (sz == 1)
    HAL::Out8((UShort)PciConfigKind::ConfigData + (fBar & 3), (UChar)data);
}

UShort Device::DeviceId() {
  return (UShort)(NewOSPCIReadRaw(0x0 >> 16, fBus, fDevice, fFunction));
}

UShort Device::VendorId() {
  return (UShort)(NewOSPCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
}

UShort Device::InterfaceId() {
  return (UShort)(NewOSPCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
}

UChar Device::Class() {
  return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 24);
}

UChar Device::Subclass() {
  return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 16);
}

UChar Device::ProgIf() {
  return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 8);
}

UChar Device::HeaderType() {
  return (UChar)(NewOSPCIReadRaw(0xC, fBus, fDevice, fFunction) >> 16);
}

void Device::EnableMmio() {
  bool enable = Read(0x04, sizeof(UChar)) | (1 << 1);
  Write(0x04, enable, sizeof(UShort));
}

void Device::BecomeBusMaster() {
  bool enable = Read(0x04, sizeof(UShort)) | (1 << 2);
  Write(0x04, enable, sizeof(UShort));
}

UShort Device::Vendor() {
  UShort vendor = VendorId();

  if (vendor != (UShort)PciConfigKind::Invalid)
    fDevice = (UShort)Read(0x0, sizeof(UShort));

  return fDevice;
}

Device::operator bool() { return VendorId() != (UShort)PciConfigKind::Invalid; }
}  // namespace NewOS::PCI
