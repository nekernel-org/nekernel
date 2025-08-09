/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <libSystem/SystemKit/Syscall.h>
#include <libSystem/SystemKit/System.h>

/// @file SystemAPI.cc
/// @brief System wide API for NeKernel.

namespace Detail {
  template<typename T>
  inline VoidPtr safe_void_cast(const T* ptr) {
    return const_cast<VoidPtr>(static_cast<const VoidPtr>(ptr));
  }
}

IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len) {
  if (!len || !dest || !src) {
    return nullptr;
  }

  auto src_bytes = static_cast<const UInt8*>(src);
  auto dst_bytes = static_cast<UInt8*>(dest);
  
  if (len >= sizeof(UInt64) && 
      (reinterpret_cast<UIntPtr>(src) % sizeof(UInt64)) == 0 &&
      (reinterpret_cast<UIntPtr>(dest) % sizeof(UInt64)) == 0) {
    
    auto src_words = static_cast<const UInt64*>(src);
    auto dst_words = static_cast<UInt64*>(dest);
    SizeT word_count = len / sizeof(UInt64);
    
    for (SizeT i = 0; i < word_count; ++i) {
      dst_words[i] = src_words[i];
    }
    
    SizeT remaining = len % sizeof(UInt64);
    for (SizeT i = 0; i < remaining; ++i) {
      dst_bytes[len - remaining + i] = src_bytes[len - remaining + i];
    }
  } else {
    for (SizeT i = 0; i < len; ++i) {
      dst_bytes[i] = src_bytes[i];
    }
  }

  return dest;
}

IMPORT_C SInt64 MmStrLen(const Char* in) {
  if (!in) return 0;

  SizeT len = 0;
  
  while (in[len] != '\0') {
    ++len;
  }

  return static_cast<SInt64>(len);
}

IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value) {
  if (!len || !dest) {
    return nullptr;
  }

  auto dst_bytes = static_cast<UInt8*>(dest);
  
  if (len >= sizeof(UInt64) && (reinterpret_cast<UIntPtr>(dest) % sizeof(UInt64)) == 0) {
    // 64-bit pattern
    UInt64 pattern = static_cast<UInt64>(value);
    pattern |= (pattern << 8);
    pattern |= (pattern << 16);
    pattern |= (pattern << 32);
    
    auto dst_words = static_cast<UInt64*>(dest);
    SizeT word_count = len / sizeof(UInt64);
    
    for (SizeT i = 0; i < word_count; ++i) {
      dst_words[i] = pattern;
    }
    
    SizeT remaining = len % sizeof(UInt64);
    for (SizeT i = 0; i < remaining; ++i) {
      dst_bytes[len - remaining + i] = value;
    }
  } else {
    for (SizeT i = 0; i < len; ++i) {
      dst_bytes[i] = value;
    }
  }

  return dest;
}

IMPORT_C Ref IoOpenFile(_Input const Char* path, _Input const Char* drv_letter) {
  return static_cast<Ref>(libsys_syscall_arg_3(
      SYSCALL_HASH("IoOpenFile"),
      Detail::safe_void_cast(path),
      Detail::safe_void_cast(drv_letter)));
}

IMPORT_C Void IoCloseFile(_Input Ref desc) {
  libsys_syscall_arg_2(SYSCALL_HASH("IoCloseFile"), static_cast<VoidPtr>(desc));
}

IMPORT_C UInt64 IoSeekFile(_Input Ref desc, _Input UInt64 off) {
  auto ret_ptr = libsys_syscall_arg_3(
      SYSCALL_HASH("IoSeekFile"), 
      static_cast<VoidPtr>(desc), 
      reinterpret_cast<VoidPtr>(&off));

  if (!ret_ptr) {
    return ~0UL;  
  }

  auto ret = static_cast<volatile UInt64*>(ret_ptr);
  UInt64 result = *ret;
  MUST_PASS(result != ~0UL);
  return result;
}

IMPORT_C UInt64 IoTellFile(_Input Ref desc) {
  auto ret_ptr = libsys_syscall_arg_2(SYSCALL_HASH("IoTellFile"),
                                      static_cast<VoidPtr>(desc));
  
  if (!ret_ptr) {
    return ~0UL; 
  }
  
  auto ret = static_cast<volatile UInt64*>(ret_ptr);
  return *ret;
}

IMPORT_C SInt32 PrintOut(_Input IORef desc, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  auto ret_ptr = libsys_syscall_arg_4(
      SYSCALL_HASH("PrintOut"), 
      static_cast<VoidPtr>(desc),
      Detail::safe_void_cast(fmt), 
      static_cast<VoidPtr>(&args));

  va_end(args);
  
  if (!ret_ptr) {
    return -1;  
  }
  
  auto ret = static_cast<const volatile SInt32*>(ret_ptr);
  return *ret;
}

IMPORT_C Void _rtl_assert(Bool expr, const Char* origin) {
  if (!expr) {
    PrintOut(nullptr, "Assertion failed: %s\r", origin);
    libsys_syscall_arg_1(SYSCALL_HASH("_rtl_debug_break"));
  }
}
