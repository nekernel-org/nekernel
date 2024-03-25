/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.hxx>

namespace System {
class HeapException;
class HeapInterface;

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

#define kAllocationTypes 2

enum HcAllocationKind {
  kStandardAllocation = 0xC,
  kArrayAllocation = 0xD,
};

CA_EXTERN_C PtrVoidType HcAllocateProcessHeap(ObjectPtr refObj, QWordType sz,
                                              DWordType flags);
CA_EXTERN_C BooleanType HcProcessHeapExists(ObjectPtr refObj, PtrVoidType ptr);
CA_EXTERN_C QWordType HcProcessHeapSize(ObjectPtr refObj, PtrVoidType ptr);
CA_EXTERN_C VoidType HcFreeProcessHeap(ObjectPtr refObj, PtrVoidType ptr);