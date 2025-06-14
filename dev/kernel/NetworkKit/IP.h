/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.h>
#include <NeKit/Defines.h>
#include <NeKit/KString.h>
#include <NeKit/Ref.h>

namespace Kernel {
class RawIPAddress6;
class RawIPAddress;
class IPFactory;

class RawIPAddress final {
 private:
  explicit RawIPAddress(UInt8 bytes[4]);
  ~RawIPAddress() = default;

  RawIPAddress& operator=(const RawIPAddress&) = delete;
  RawIPAddress(const RawIPAddress&)            = default;

 public:
  UInt8* Address();

  UInt8& operator[](const Size& index);

  BOOL operator==(const RawIPAddress& ipv6);
  BOOL operator!=(const RawIPAddress& ipv6);

 private:
  UInt8 fAddr[4];

  friend IPFactory;  // it is the one creating these addresses, thus this
                     // is why the constructors are private.
};

/**
 * @brief IPv6 address.
 */
class RawIPAddress6 final {
 private:
  explicit RawIPAddress6(UInt8 Bytes[16]);
  ~RawIPAddress6() = default;

  RawIPAddress6& operator=(const RawIPAddress6&) = delete;
  RawIPAddress6(const RawIPAddress6&)            = default;

 public:
  UInt8* Address() { return fAddr; }

  UInt8& operator[](const Size& index);

  bool operator==(const RawIPAddress6& ipv6);
  bool operator!=(const RawIPAddress6& ipv6);

 private:
  UInt8 fAddr[16];

  friend IPFactory;
};

/**
 * @brief IP Creation helpers
 */
class IPFactory final {
 public:
  static ErrorOr<KString> ToKString(Ref<RawIPAddress6>& ipv6);
  static ErrorOr<KString> ToKString(Ref<RawIPAddress>& ipv4);
  static bool             IpCheckVersion4(const Char* ip);
};
}  // namespace Kernel
