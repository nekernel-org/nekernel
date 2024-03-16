/* -------------------------------------------

    Copyright Mahrouss Logic

    File: PS2KernelMouse.hxx
    Purpose: PS/2 mouse.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

namespace HCore {
/// @brief Enabled for kernel purposes, kernel mouse.
class PS2KernelMouse final {
 public:
  explicit PS2KernelMouse() = default;
  ~PS2KernelMouse() = default;

  HCORE_COPY_DEFAULT(PS2KernelMouse);

 public:
  Void Init() noexcept {
    HCore::kcout << "HCoreKrnl.exe: Enabling PS/2 mouse...\r\n";

    this->Write(0xFF);

    this->Wait();

    HAL::Out8(0x64, 0x20);

    this->Wait();

    auto status = HAL::In8(0x60);

    status |= 0x12;

    this->Wait();
    
    HAL::Out8(0x64, 0x60);

    this->Wait();
    
    HAL::Out8(0x60, status);

    HCore::kcout << "HCoreKrnl.exe: PS/2 mouse is OK.\r\n";
  }

  private:
  Bool WaitInput() noexcept {
    UInt64 timeout = 100000;

    while (timeout) {
      if ((HAL::In8(0x64) & 0x1)) {
        HCore::kcout << "HCoreKrnl.exe: Wait: OK\r\n";
        return true;
      }

      --timeout;
    }  // wait until we can read

    HCore::kcout << "HCoreKrnl.exe: Wait: Timeout\r\n";
    // return the ack bit.
    return false;
  }

  Bool Wait() noexcept {
    UInt64 timeout = 100000;

    while (timeout) {
      if ((HAL::In8(0x64) & 0b10) == 0) {
        HCore::kcout << "HCoreKrnl.exe: Wait: OK\r\n";
        return true;
      }

      --timeout;
    }  // wait until we can read

    HCore::kcout << "HCoreKrnl.exe: Wait: Timeout\r\n";
    // return the ack bit.
    return false;
  }

  Void Write(UInt8 val) {
    this->Wait();
    HAL::Out8(0x64, 0xD4);
    this->Wait();

    HAL::Out8(0x60, val);
  }

  UInt8 Read() {
    this->WaitInput();
    return HAL::In8(0x60);
  }
};
}  // namespace HCore
