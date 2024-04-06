/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/**
 * @file AHCI.cxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Mahrouss Logic
 *
 */

#include <Builtins/AHCI/Defines.hxx>
#include <KernelKit/PCI/Iterator.hpp>

#ifdef __AHCI__
enum { kSATAProgIfAHCI = 0x01, kSATASubClass = 0x06 };

static NewOS::PCI::Device kAhciDevice;

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of port that have been detected.
/// @return
NewOS::Boolean drv_std_init(NewOS::UInt16& PortsImplemented) {
  using namespace NewOS;

  PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);
  for (SizeT devIndex = 0; devIndex < NEWOS_BUS_COUNT; ++devIndex) {
    if (iterator[devIndex].Leak().Subclass() == kSATASubClass &&
        iterator[devIndex].Leak().ProgIf() == kSATAProgIfAHCI) {
      iterator[devIndex].Leak().EnableMmio();
      kAhciDevice = iterator[devIndex].Leak();

      kcout << "NewKernel: Found AHCI controller.\r\n";
    
      return true;
    }
  }

  return false;
}

NewOS::Boolean drv_std_detected(NewOS::Void) {
  return kAhciDevice.DeviceId() != 0xFFFF;
}

NewOS::Void drv_std_read(NewOS::UInt64 Lba, NewOS::Char* Buf,
                          NewOS::SizeT SectorSz, NewOS::SizeT Size) {}

NewOS::Void drv_std_write(NewOS::UInt64 Lba, NewOS::Char* Buf,
                           NewOS::SizeT SectorSz, NewOS::SizeT Size) {}
#endif  // __AHCI__