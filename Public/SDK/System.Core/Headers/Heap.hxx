/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.hxx>

namespace System {
class MemoryException;

typedef PtrVoidType PtrHeapType;

enum {
  kHeapExpandable = 2,
  kHeapNoExecute = 4,
  kHeapShared = 6,
  kHeapReadOnly = 8,
  kHeapNoFlags = 0
};

class HeapInterface final {
 private:
  explicit HeapInterface();

 public:
  ~HeapInterface();

 public:
  CA_COPY_DEFAULT(HeapInterface);

 public:
  static HeapInterface *Shared() noexcept;

 public:
  void Delete(PtrHeapType me) noexcept;
  SizeType Size(PtrHeapType me) noexcept;
  PtrHeapType New(const SizeType &sz, 
  const DWordType flags = kHeapNoFlags);
};


/// @brief heap exception
/// Throws when the heap pointer isn't found or invalid.
class HeapException : public SystemException {
 public:
  explicit HeapException() = default;
  virtual ~HeapException() = default;

 public:
  CA_COPY_DEFAULT(HeapException);

 public:
  const char *Name() override { return "HeapException"; }
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{"System.Core: HeapException: Catastrophic failure!"};
};

}  // namespace System