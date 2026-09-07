// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef KERNELKIT_IDYLIBOBJECT_H
#define KERNELKIT_IDYLIBOBJECT_H

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Config.h>

#define NE_DYLIB_OBJECT : public IDylibObject

namespace Ne::Kernel {

class IDylibObject;

/// @brief Dylib class object. A handle to a shared library.
class IDylibObject {
 public:
  explicit IDylibObject() = default;
  virtual ~IDylibObject() = default;

  struct DylibTraits final {
    VoidPtr ImageObject{nullptr};
    SizeT   ImageSz{0UL};
    VoidPtr ImageEntrypointOffset{nullptr};

    VoidPtr Image() const { return ImageObject; }
    Bool    IsValid() const { return ImageObject && ImageEntrypointOffset; }
  };

  NE_COPY_DEFAULT(IDylibObject)

  virtual DylibTraits** GetAddressOf() { return nullptr; }
  virtual DylibTraits*  Get() { return nullptr; }

  virtual Void Mount(DylibTraits*) {}
  virtual Void Unmount() {}
};

/// @brief Pure implementation, missing method/function handler.
EXTERN_C void __ne_pure_call(void);

}  // namespace Ne::Kernel

#endif
