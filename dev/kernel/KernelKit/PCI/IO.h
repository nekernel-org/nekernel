/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <NeKit/Array.h>
#include <NeKit/Defines.h>
#include <NeKit/Ref.h>

namespace Kernel {
template <SizeT Sz>
class IOArray final {
 public:
  IOArray() = delete;

  IOArray(nullPtr) = delete;

  explicit IOArray(Array<UShort, Sz>& ports) : fPorts(ports) {}

  ~IOArray() {}

  IOArray& operator=(const IOArray&) = default;

  IOArray(const IOArray&) = default;

  operator bool() { return !fPorts.Empty(); }

 public:
  template <typename T>
  T In(SizeT index);

  template <typename T>
  void Out(SizeT index, T value);

 private:
  Array<UShort, Sz> fPorts;
};

inline constexpr UInt16 kMaxPorts = 16;

using IOArray16 = IOArray<kMaxPorts>;

template <SizeT Sz>
inline Array<UShort, Sz> make_ports(UShort base) {
  Array<UShort, Sz> ports;

  for (UShort i = 0; i < Sz; ++i) {
    ports[i] = base + i;
  }

  return ports;
}
}  // namespace Kernel

#ifdef __NE_AMD64__
#include <KernelKit/PCI/IOArray+AMD64.inl>
#else
#error Please provide platform specific code for the I/O
#endif  // ifdef __NE_AMD64__
