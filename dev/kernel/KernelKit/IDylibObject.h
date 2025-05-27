/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>

#define NE_DYLIB_OBJECT : public IDylibObject

namespace Kernel {
class IDylibObject;

/// @brief Dylib class object. A handle to a shared library.
class IDylibObject {
 public:
  explicit IDylibObject() = default;
  virtual ~IDylibObject() = default;

  struct DylibTraits final {
    VoidPtr ImageObject{nullptr};
    VoidPtr ImageEntrypointOffset{nullptr};

    VoidPtr Image() const { return ImageObject; }
    Bool    IsValid() const { return ImageObject && ImageEntrypointOffset; }
  };

  NE_COPY_DEFAULT(IDylibObject)

  virtual DylibTraits** GetAddressOf() = 0;
  virtual DylibTraits*  Get()          = 0;

  virtual Void Mount(DylibTraits* to_mount) = 0;
  virtual Void Unmount()                    = 0;
};

/// @brief Pure implementation, missing method/function handler.
EXTERN_C void __zka_pure_call(void);
}  // namespace Kernel
