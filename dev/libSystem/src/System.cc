/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <libSystem/SystemKit/Err.h>
#include <libSystem/SystemKit/Syscall.h>
#include <libSystem/SystemKit/System.h>

namespace Detail {
template <typename T>
static VoidPtr safe_void_cast(const T* ptr) {
  _rtl_assert(ptr, "safe void cast failed!");
  return static_cast<VoidPtr>(const_cast<T*>(ptr));
}
}  // namespace Detail

IMPORT_C Void _rtl_assert(Bool expr, const Char* origin) {
  if (!expr) {
    PrintOut(nullptr, "Assertion failed: %s\r", origin);
    libsys_syscall_arg_1(SYSCALL_HASH("_rtl_debug_break"));
  }
}

/// @note this uses the FNV 64-bit variant.
IMPORT_C UInt64 libsys_hash_64(const Char* path) {
  if (!path || *path == 0) return 0;

  const UInt64 kFNVSeed  = 0xcbf29ce484222325ULL;
  const UInt64 kFNVPrime = 0x100000001b3ULL;

  UInt64 hash = kFNVSeed;

  while (*path) {
    hash ^= (Char) (*path++);
    hash *= kFNVPrime;
  }

  return hash;
}

// memmove-style copy
IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len) {
  // handles overlap, prefers 64-bit word copies when aligned
  if (!len || !dest || !src) return nullptr;

  auto s = static_cast<const UInt8*>(src);
  auto d = static_cast<UInt8*>(dest);

  if (d == s) return dest;

  // decide direction
  if (d > s && d < s + len) {
    const UInt8* rs = s + len;
    UInt8*       rd = d + len;

    // try 64-bit aligned backward copy
    if (len >= sizeof(UInt64) && (reinterpret_cast<UIntPtr>(rs) % sizeof(UInt64) == 0) &&
        (reinterpret_cast<UIntPtr>(rd) % sizeof(UInt64) == 0)) {
      auto  rsw   = reinterpret_cast<const UInt64*>(rs);
      auto  rdw   = reinterpret_cast<UInt64*>(rd);
      SizeT words = len / sizeof(UInt64);

      for (SizeT i = 0; i < words; ++i) {
        rdw[-1 - static_cast<SizeT>(i)] = rsw[-1 - static_cast<SizeT>(i)];
      }

      SizeT rem = len % sizeof(UInt64);
      for (SizeT i = 0; i < rem; ++i) {
        rd[-1 - i] = rs[-1 - i];
      }
    } else {
      // byte-wise backward
      for (SizeT i = 0; i < len; ++i) {
        rd[-1 - i] = rs[-1 - i];
      }
    }
  } else {
    // try 64-bit aligned forward copy
    if (len >= sizeof(UInt64) && (reinterpret_cast<UIntPtr>(s) % sizeof(UInt64) == 0) &&
        (reinterpret_cast<UIntPtr>(d) % sizeof(UInt64) == 0)) {
      auto  sw    = reinterpret_cast<const UInt64*>(s);
      auto  dw    = reinterpret_cast<UInt64*>(d);
      SizeT words = len / sizeof(UInt64);

      for (SizeT i = 0; i < words; ++i) {
        dw[i] = sw[i];
      }

      SizeT       rem    = len % sizeof(UInt64);
      const SizeT offset = words * sizeof(UInt64);
      for (SizeT i = 0; i < rem; ++i) {
        d[offset + i] = s[offset + i];
      }
    } else {
      for (SizeT i = 0; i < len; ++i) {
        d[i] = s[i];
      }
    }
  }

  return dest;
}

IMPORT_C SInt64 MmStrLen(const Char* in) {
  // strlen via pointer walk
  if (!in) return 0;
  const Char* p = in;
  while (*p) ++p;
  return static_cast<SInt64>(p - in);
}

IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value) {
  if (!len || !dest) return nullptr;

  auto d = static_cast<UInt8*>(dest);

  if (len >= sizeof(UInt64) && (reinterpret_cast<UIntPtr>(d) % sizeof(UInt64)) == 0) {
    UInt64 pattern = static_cast<UInt64>(value);
    pattern |= (pattern << 8);
    pattern |= (pattern << 16);
    pattern |= (pattern << 32);

    auto  dw    = reinterpret_cast<UInt64*>(d);
    SizeT words = len / sizeof(UInt64);

    for (SizeT i = 0; i < words; ++i) {
      dw[i] = pattern;
    }

    SizeT       rem    = len % sizeof(UInt64);
    const SizeT offset = words * sizeof(UInt64);
    for (SizeT i = 0; i < rem; ++i) {
      d[offset + i] = value;
    }
  } else {
    for (SizeT i = 0; i < len; ++i) d[i] = value;
  }

  return dest;
}

IMPORT_C Ref IoOpenFile(_Input const Char* path, _Input const Char* drv_letter) {
  return static_cast<Ref>(libsys_syscall_arg_3(SYSCALL_HASH("IoOpenFile"),
                                               Detail::safe_void_cast(path),
                                               Detail::safe_void_cast(drv_letter)));
}

IMPORT_C Void IoCloseFile(_Input Ref desc) {
  libsys_syscall_arg_2(SYSCALL_HASH("IoCloseFile"), static_cast<VoidPtr>(desc));
}

IMPORT_C UInt64 IoSeekFile(_Input Ref desc, _Input UInt64 off) {
  auto ret_ptr = libsys_syscall_arg_3(SYSCALL_HASH("IoSeekFile"), static_cast<VoidPtr>(desc),
                                      reinterpret_cast<VoidPtr>(&off));

  if (!ret_ptr) return ~0UL;

  auto   ret    = static_cast<volatile UInt64*>(ret_ptr);
  UInt64 result = *ret;
  MUST_PASS(result != ~0UL);
  return result;
}

IMPORT_C UInt64 IoTellFile(_Input Ref desc) {
  auto ret_ptr = libsys_syscall_arg_2(SYSCALL_HASH("IoTellFile"), static_cast<VoidPtr>(desc));
  if (!ret_ptr) return ~0UL;
  auto ret = static_cast<volatile UInt64*>(ret_ptr);
  return *ret;
}

IMPORT_C SInt32 PrintOut(_Input IORef desc, const Char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  auto buf = StrFmt(fmt, args);

  va_end(args);

  // if truncated, `needed` >= kBufferSz; we still send truncated buffer
  auto ret_ptr = libsys_syscall_arg_3(SYSCALL_HASH("PrintOut"), static_cast<VoidPtr>(desc),
                                      Detail::safe_void_cast(buf));

  if (!ret_ptr) return -kErrorInvalidData;

  auto ret = static_cast<const volatile SInt32*>(ret_ptr);

  return *ret;
}
