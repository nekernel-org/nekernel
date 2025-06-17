/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NeKit/Utils.h>
#include <NetworkKit/IP.h>

namespace Kernel {
UInt8* RawIPAddress::Address() {
  return fAddr;
}

RawIPAddress::RawIPAddress(UInt8 bytes[4]) {
  rt_copy_memory(bytes, fAddr, 4);
}

BOOL RawIPAddress::operator==(const RawIPAddress& ipv4) {
  for (Size index = 0; index < 4; ++index) {
    if (ipv4.fAddr[index] != fAddr[index]) return false;
  }

  return true;
}

BOOL RawIPAddress::operator!=(const RawIPAddress& ipv4) {
  for (Size index = 0; index < 4; ++index) {
    if (ipv4.fAddr[index] == fAddr[index]) return false;
  }

  return true;
}

UInt8& RawIPAddress::operator[](const Size& index) {
  kout << "[RawIPAddress::operator[]] Fetching Index...\r";

  static UInt8 IP_PLACEHOLDER = '0';
  if (index >= 4) return IP_PLACEHOLDER;

  return fAddr[index];
}

RawIPAddress6::RawIPAddress6(UInt8 bytes[16]) {
  rt_copy_memory(bytes, fAddr, 16);
}

UInt8& RawIPAddress6::operator[](const Size& index) {
  kout << "[RawIPAddress6::operator[]] Fetching Index...\r";

  static UInt8 IP_PLACEHOLDER = '0';
  if (index >= 16) return IP_PLACEHOLDER;

  return fAddr[index];
}

bool RawIPAddress6::operator==(const RawIPAddress6& ipv6) {
  for (SizeT index = 0; index < 16; ++index) {
    if (ipv6.fAddr[index] != fAddr[index]) return false;
  }

  return true;
}

bool RawIPAddress6::operator!=(const RawIPAddress6& ipv6) {
  for (SizeT index = 0; index < 16; ++index) {
    if (ipv6.fAddr[index] == fAddr[index]) return false;
  }

  return true;
}

/// @todo
ErrorOr<KString> IPFactory::ToKString(Ref<RawIPAddress6>& ipv6) {
  NE_UNUSED(ipv6);
  auto str = KStringBuilder::Construct(0);
  return str;
}

/// @todo
ErrorOr<KString> IPFactory::ToKString(Ref<RawIPAddress>& ipv4) {
  NE_UNUSED(ipv4);
  auto str = KStringBuilder::Construct(0);
  return str;
}

/// @note Doesn't catch IPs such as 256.999.0.1, UNSAFE!
bool IPFactory::IpCheckVersion4(const Char* ip) {
  if (!ip) return NO;

  Int32 cnter = 0;
  Int32 dot_cnter = 0;

  for (SizeT base = 0; base < rt_string_len(ip); ++base) {
    if (ip[base] == '.') {
      cnter = 0;
      ++dot_cnter;
    } else {
      if (!rt_is_alnum(ip[base])) return NO;
      if (cnter == 3) return NO;

      ++cnter;
    }
  }

  if (dot_cnter != 3) return NO;

  return YES;
}
}  // namespace Kernel
