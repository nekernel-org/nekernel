/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/PCI/Device.h>
#include <NeKit/Array.h>
#include <NeKit/OwnPtr.h>
#include <NeKit/Ref.h>

namespace Kernel {
enum class DmaKind {
  PCI = 10,  // Bus mastering is required to be turned on. Basiaclly a request
  // control system. 64-Bit access depends on the PAE bit and the device
  // (if Double Address Cycle is available)
  ISA,  // Four DMA channels 0-3; 8 bit transfers and only a megabyte of RAM.
  Count   = 2,
  Invalid = 0,
};

class DMAWrapper final {
 public:
  explicit DMAWrapper() = delete;

 public:
  explicit DMAWrapper(nullPtr) = delete;
  explicit DMAWrapper(voidPtr Ptr, DmaKind Kind = DmaKind::PCI) : fAddress(Ptr), fKind(Kind) {}

 public:
  DMAWrapper& operator=(voidPtr Ptr);

 public:
  DMAWrapper& operator=(const DMAWrapper&) = default;
  DMAWrapper(const DMAWrapper&)            = default;

 public:
  ~DMAWrapper() = default;

  template <class T>
  T* operator->();

  template <class T>
  T* Get(UIntPtr off = 0);

 public:
       operator bool();
  bool operator!();

 public:
  bool    Write(UIntPtr& bit, const UInt32& offset);
  UIntPtr Read(const UInt32& offset);
  Boolean Check(UIntPtr offset) const;

 public:
  UIntPtr operator[](UIntPtr& offset);

 private:
  voidPtr fAddress{nullptr};
  DmaKind fKind{DmaKind::Invalid};

 private:
  friend class DMAFactory;
};

class DMAFactory final {
 public:
  static OwnPtr<IOBuf<Char*>> Construct(OwnPtr<DMAWrapper>& dma);
};
}  // namespace Kernel

#include <KernelKit/PCI/DMA.inl>
