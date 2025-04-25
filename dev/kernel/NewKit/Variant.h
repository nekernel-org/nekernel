/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/Json.h>
#include <NewKit/KString.h>
#include <SwapKit/DiskSwap.h>

namespace Kernel {
class Variant final {
 public:
  enum class VariantKind {
    kString,
    kBlob,
    kNull,
    kJson,
    kXML,
    kSwap,
    kInvalid,
  };

 public:
  explicit Variant() = delete;

 public:
  NE_COPY_DEFAULT(Variant)

  ~Variant() = default;

 public:
  explicit Variant(KString* stringView) : fPtr((VoidPtr) stringView), fKind(VariantKind::kString) {}

  explicit Variant(Json* json) : fPtr((VoidPtr) json), fKind(VariantKind::kJson) {}

  explicit Variant(nullPtr ptr) : fPtr(ptr), fKind(VariantKind::kNull) {}

  explicit Variant(SWAP_DISK_HEADER* ptr) : fPtr(ptr), fKind(VariantKind::kSwap) {}

  explicit Variant(VoidPtr ptr) : fPtr(ptr), fKind(VariantKind::kBlob) {}

 public:
  const Char* ToString();
  VoidPtr     Leak();

  template <typename T>
  T* As() {
    return reinterpret_cast<T*>(fPtr);
  }

  VariantKind& Kind();

 private:
  voidPtr     fPtr{nullptr};
  VariantKind fKind{VariantKind::kNull};
};
}  // namespace Kernel
