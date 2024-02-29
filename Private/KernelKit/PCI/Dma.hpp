/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <KernelKit/PCI/Device.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/Ref.hpp>

namespace HCore {
enum class DmaKind {
  PCI,  // Bus mastering is required to be turned on. Basiaclly a request
  // control system. 64-Bit access depends on the PAE bit and the device
  // (if Double Address Cycle is available)
  ISA,  // Four DMA channels 0-3; 8 bit transfers and only a megabyte of RAM.
  Invalid,
};

class DMAWrapper final {
 public:
  explicit DMAWrapper() = delete;

 public:
  explicit DMAWrapper(nullPtr) = delete;
  explicit DMAWrapper(voidPtr Ptr, DmaKind Kind = DmaKind::PCI)
      : m_Address(Ptr), m_Kind(Kind) {}

 public:
  DMAWrapper &operator=(voidPtr Ptr);

 public:
  DMAWrapper &operator=(const DMAWrapper &) = default;
  DMAWrapper(const DMAWrapper &) = default;

 public:
  ~DMAWrapper() = default;

  template <class T>
  T *operator->();

  template <class T>
  T *Get(const UIntPtr off = 0);

 public:
  operator bool();
  bool operator!();

 public:
  bool Write(const UIntPtr &bit, const UIntPtr &offset);
  UIntPtr Read(const UIntPtr &offset);
  Boolean Check(UIntPtr offset) const;

 public:
  UIntPtr operator[](const UIntPtr &offset);

 private:
  voidPtr m_Address{nullptr};
  DmaKind m_Kind{DmaKind::Invalid};

 private:
  friend class DMAFactory;
};

class DMAFactory final {
 public:
  static OwnPtr<IOBuf<Char *>> Construct(OwnPtr<DMAWrapper> &dma);
};
}  // namespace HCore

#include <KernelKit/PCI/Dma.inl>
