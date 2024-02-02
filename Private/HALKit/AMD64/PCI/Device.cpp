/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/PCI/Device.hpp>

HCore::UInt LumiaPCIReadRaw(HCore::UInt bar, HCore::UShort bus,
                            HCore::UShort dev, HCore::UShort fun) {
  HCore::UInt target = 0x80000000 | ((HCore::UInt)bus << 16) |
                       ((HCore::UInt)dev << 11) | ((HCore::UInt)fun << 8) |
                       (bar & 0xFC);

  HCore::HAL::Out32((HCore::UShort)HCore::PCI::PciConfigKind::ConfigAddress,
                    target);

  return HCore::HAL::In32((HCore::UShort)HCore::PCI::PciConfigKind::ConfigData);
}

void LumiaPCISetCfgTarget(HCore::UInt bar, HCore::UShort bus, HCore::UShort dev,
                          HCore::UShort fun) {
  HCore::UInt target = 0x80000000 | ((HCore::UInt)bus << 16) |
                       ((HCore::UInt)dev << 11) | ((HCore::UInt)fun << 8) |
                       (bar & ~3);

  HCore::HAL::Out32((HCore::UShort)HCore::PCI::PciConfigKind::ConfigAddress,
                    target);
}

namespace HCore::PCI {
Device::Device(UShort bus, UShort device, UShort func, UShort bar)
    : m_Bus(bus), m_Device(device), m_Function(func), m_Bar(bar) {}

Device::~Device() {}

UInt Device::Read(UInt bar, Size sz) {
  LumiaPCISetCfgTarget(bar, m_Bus, m_Device, m_Function);

  if (sz == 4)
    return HAL::In32((UShort)PciConfigKind::ConfigData + (m_Bar & 3));
  if (sz == 2)
    return HAL::In16((UShort)PciConfigKind::ConfigData + (m_Bar & 3));
  if (sz == 1) return HAL::In8((UShort)PciConfigKind::ConfigData + (m_Bar & 3));

  return 0xFFFF;
}

void Device::Write(UInt bar, UIntPtr data, Size sz) {
  LumiaPCISetCfgTarget(bar, m_Bus, m_Device, m_Function);

  if (sz == 4)
    HAL::Out32((UShort)PciConfigKind::ConfigData + (m_Bar & 3), (UInt)data);
  if (sz == 2)
    HAL::Out16((UShort)PciConfigKind::ConfigData + (m_Bar & 3), (UShort)data);
  if (sz == 1)
    HAL::Out8((UShort)PciConfigKind::ConfigData + (m_Bar & 3), (UChar)data);
}

UShort Device::DeviceId() {
  return (UShort)(LumiaPCIReadRaw(0x0 >> 16, m_Bus, m_Device, m_Function));
}

UShort Device::VendorId() {
  return (UShort)(LumiaPCIReadRaw(0x0, m_Bus, m_Device, m_Function) >> 16);
}

UShort Device::InterfaceId() {
  return (UShort)(LumiaPCIReadRaw(0x0, m_Bus, m_Device, m_Function) >> 16);
}

UChar Device::Class() {
  return (UChar)(LumiaPCIReadRaw(0x08, m_Bus, m_Device, m_Function) >> 24);
}

UChar Device::Subclass() {
  return (UChar)(LumiaPCIReadRaw(0x08, m_Bus, m_Device, m_Function) >> 16);
}

UChar Device::ProgIf() {
  return (UChar)(LumiaPCIReadRaw(0x08, m_Bus, m_Device, m_Function) >> 8);
}

UChar Device::HeaderType() {
  return (UChar)(LumiaPCIReadRaw(0xC, m_Bus, m_Device, m_Function) >> 16);
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
    m_Device = (UShort)Read(0x0, sizeof(UShort));

  return m_Device;
}

Device::operator bool() { return VendorId() != (UShort)PciConfigKind::Invalid; }
}  // namespace HCore::PCI
