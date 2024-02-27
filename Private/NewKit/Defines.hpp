/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Macros.hpp>

#define NEWKIT_VERSION "1.0.0"

#if !defined(_INC_NO_STDC_HEADERS) && defined(__GNUC__)
#include <CRT/__cxxkit_defines.hxx>
#endif

#ifdef __has_feature
#if !__has_feature(cxx_nullptr)
#error You must at least have nullptr featured on your C++ compiler.
#endif
#endif

namespace HCore {
using voidPtr = void *;
using VoidPtr = void *;
using nullPtr = decltype(nullptr);
using NullPtr = decltype(nullptr);

using Int = int;
using Int32 = int;
using UShort = unsigned short;
using UInt16 = unsigned short;
using Short = short;
using Int16 = short;
using UInt = unsigned int;
using UInt32 = unsigned int;
using Long = __INT64_TYPE__;
using Int64 = __INT64_TYPE__;
using ULong = __UINT64_TYPE__;
using UInt64 = __UINT64_TYPE__;
using Boolean = bool;
using Bool = bool;
using Char = char;
using UChar = unsigned char;
using UInt8 = unsigned char;

using SSize = Int64;
using SSizeT = Int64;
using Size = __SIZE_TYPE__;
using SizeT = __SIZE_TYPE__;
using IntPtr = __INTPTR_TYPE__;
using UIntPtr = __UINTPTR_TYPE__;
using IntFast = __INT_FAST32_TYPE__;
using IntFast64 = __INT_FAST64_TYPE__;
using PtrDiff = __PTRDIFF_TYPE__;

typedef UIntPtr *Ptr64;
typedef UInt32 *Ptr32;

using Utf8Char = char8_t;
using Utf16Char = char16_t;
using WideChar = wchar_t;
using Utf32Char = char32_t;

using Void = void;

using Lba = SSizeT;

enum class Endian : UChar { kLittle, kBig };

template <typename Args>
Args &&forward(Args &arg) {
  return static_cast<Args &&>(arg);
}

template <typename Args>
Args &&move(Args &&arg) {
  return static_cast<Args &&>(arg);
}

typedef UIntPtr _HandleCls;

/// @brief Looks like an objc class, because it takes inspiration from it.
typedef struct _Handle {
  _HandleCls _Cls;
  void Release(_Handle *);
  _Handle* Init();
  _Handle* Sel(_Handle*, ...);
} *Handle;
}  // namespace HCore

#define DEDUCE_ENDIAN(address, value)                         \
  (((reinterpret_cast<HCore::Char *>(address)[0]) == (value)) \
       ? (HCore::Endian::kBig)                                \
       : (HCore::Endian::kLittle))

#define Yes (true)
#define No (false)

#define VoidStar HCore::voidPtr

#ifdef INIT
#undef INIT
#endif  // ifdef INIT

#define INIT(OBJ, TYPE, ...) TYPE OBJ = TYPE(__VA_ARGS__)
