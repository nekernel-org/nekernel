/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *      ========================================================
 */

#ifndef __KERNELKIT_PEF_SHARED_OBJECT_H__
#define __KERNELKIT_PEF_SHARED_OBJECT_H__

#include <KernelKit/IDylibObject.h>
#include <KernelKit/PEF.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/Defines.h>

namespace Kernel {
/**
 * @brief Shared Library class
 * Load library from this class
 */
class IPEFDylibObject final NE_DYLIB_OBJECT {
 public:
  explicit IPEFDylibObject() = default;
  ~IPEFDylibObject()         = default;

 public:
  NE_COPY_DEFAULT(IPEFDylibObject)

 private:
  DylibTraits* fMounted{nullptr};

 public:
  DylibTraits** GetAddressOf() { return &fMounted; }

  DylibTraits* Get() { return fMounted; }

 public:
  void Mount(DylibTraits* to_mount) noexcept {
    if (!to_mount || !to_mount->ImageObject) return;

    fMounted = to_mount;

    if (fLoader && to_mount) {
      delete fLoader;
      fLoader = nullptr;
    }

    if (!fLoader) {
      fLoader = new PEFLoader(fMounted->ImageObject);
    }
  }

  void Unmount() noexcept {
    if (fMounted) fMounted = nullptr;
  };

  template <typename SymbolType>
  SymbolType Load(const Char* symbol_name, const SizeT& len, const UInt32& kind) {
    if (symbol_name == nullptr || *symbol_name == 0) return nullptr;
    if (len > kPathLen || len < 1) return nullptr;

    auto ret = reinterpret_cast<SymbolType>(fLoader->FindSymbol(symbol_name, kind).Leak().Leak());

    if (!ret) {
      if (kind == kPefCode) return (VoidPtr) &__ne_pure_call;

      return nullptr;
    }

    return ret;
  }

 private:
  PEFLoader* fLoader{nullptr};
};

typedef IPEFDylibObject* IDylibRef;

EXTERN_C IDylibRef rtl_init_dylib_pef(USER_PROCESS& header);
EXTERN_C Void      rtl_fini_dylib_pef(USER_PROCESS& header, IDylibRef lib, Bool* successful);
}  // namespace Kernel

#endif /* ifndef __KERNELKIT_PEF_SHARED_OBJECT_H__ */
