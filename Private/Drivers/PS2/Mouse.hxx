/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Mouse.hxx
    Purpose: PS/2 mouse.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

namespace HCore {

class PS2Mouse final {
 public:
  explicit PS2Mouse() {
    HAL::Out8(0x64, 0xa8);
    this->Wait();
    auto stat = HAL::In8(0x60);

    stat |= 0b10;

    this->Wait();

    HAL::Out8(0x64, 0x60);

    this->Wait();

    HAL::Out8(0x60, stat);

    this->Write(0xF6);
    this->Read();

    this->Write(0xF4);
    this->Read();
  }

  ~PS2Mouse() = default;

  HCORE_COPY_DEFAULT(PS2Mouse);

  struct PS2MouseTraits final {
    Int16 Status;
    Int32 X, Y;
  };

  Boolean operator>>(PS2MouseTraits& stat) noexcept { return true; }

 private:
  Bool Wait() noexcept {
    while (!(HAL::In8(0x64) & 1)) {
      asm("pause");
    }  // wait until we can read

    // return the ack bit.
    return HAL::In8(0x64);
  }

  Void Write(UInt8 port) {
    this->Wait();
    HAL::Out8(0x64, 0xD4);
    this->Wait();

    HAL::Out8(0x60, port);
  }

  UInt8 Read() {
    this->Wait();
    return HAL::In8(0x60);
  }
};
}  // namespace HCore
