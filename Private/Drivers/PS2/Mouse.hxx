/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Mouse.hxx
    Purpose: PS/2 mouse.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/Defines.hpp>

namespace HCore {

class PS2Mouse {
  explicit PS2Mouse() = default;
  ~PS2Mouse() = default;

  HCORE_COPY_DEFAULT(PS2Mouse);

  struct PS2MouseTraits final {
    Int16 Status;
    Int32 X, Y;
  };

  PS2MouseTraits Read() noexcept {
    PS2MouseTraits stat;

    return stat;
  }

 private:
  UInt8 Wait() {
    while (!(HAL::In8(0x64) & 1)) {
      asm("pause");
    }  // wait until we can read

    // return the ack bit.
    return HAL::In8(0x64);
  }
};
}  // namespace HCore
