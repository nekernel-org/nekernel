/* -------------------------------------------

    Copyright Mahrouss Logic

    File: PS2MouseInterface.hxx
    Purpose: PS/2 mouse.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

namespace HCore {
/// @brief PS/2 Mouse driver interface
class PS2MouseInterface final {
 public:
  explicit PS2MouseInterface() = default;
  ~PS2MouseInterface() = default;

  HCORE_COPY_DEFAULT(PS2MouseInterface);

 public:
  Void Init() noexcept {
    HAL::rt_cli();

    HCore::kcout << "HCoreKrnl.exe: Enabling PS/2 mouse...\r\n";

    this->Write(0xFF);

    HAL::Out8(0x64, 0xA8);

    this->Wait();

    HAL::Out8(0x64, 0x20);

    this->WaitInput();

    UInt8 dataStatus = HAL::In8(0x60);

    dataStatus |= 0b10;

    this->Wait();

    HAL::Out8(0x60, dataStatus);

    this->Write(0xF6);
    auto f6Dat = this->Read();

    this->Write(0xF4);
    auto f4Dat = this->Read();

    HCore::kcout << "HCoreKrnl.exe: PS/2 mouse is OK: " << hex_number(f6Dat);
    HCore::kcout << ", " << hex_number(f4Dat) << end_line();
    
    HAL::Out8(0x64, 0xAD);

    HAL::rt_sti();
  }

 public:
  Bool WaitInput() noexcept {
    UInt64 timeout = 100000;

    while (timeout) {
      if ((HAL::In8(0x64) & 0x1) == 0x0) {
        HCore::kcout << "NewKernel.exe: Wait: OK\r\n";
        return true;
      }

      --timeout;
    }  // wait until we can read

    HCore::kcout << "NewKernel.exe: Wait: Timeout\r\n";
    // return the ack bit.
    return false;
  }

  Bool Wait() noexcept {
    UInt64 timeout = 100000;

    while (timeout) {
      if ((HAL::In8(0x64) & 0b10) == 0) {
        HCore::kcout << "NewKernel.exe: Wait: OK\r\n";
        return true;
      }

      --timeout;
    }  // wait until we can read

    HCore::kcout << "NewKernel.exe: Wait: Timeout\r\n";
    // return the ack bit.
    return false;
  }

  Void Write(UInt8 val) {
    HAL::Out8(0x64, 0xD4);
    this->Wait();
    HAL::Out8(0x60, val);
    this->Wait();
  }

  UInt8 Read() {
    this->WaitInput();
    return HAL::In8(0x60);
  }
};
}  // namespace HCore
