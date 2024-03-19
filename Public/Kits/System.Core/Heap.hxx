/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <System.Core/Defs.hxx>

#define kAllocationTypes 2

CA_EXTERN_C PVOID HcAllocateProcessHeap(ObjectPtr refObj, QWORD sz,
                                        DWORD flags);
CA_EXTERN_C BOOL HcProcessHeapExists(ObjectPtr refObj, PVOID ptr);
CA_EXTERN_C QWORD HcProcessHeapSize(ObjectPtr refObj, PVOID ptr);
CA_EXTERN_C VOID HcFreeProcessHeap(ObjectPtr refObj, PVOID ptr);
CA_EXTERN_C ObjectPtr HcGetProcessObject(void);

enum HcAllocationKind {
  kStandardAllocation = 0xC,
  kArrayAllocation = 0xD,
};

namespace System {
using namespace HCore;

class MemoryException;

typedef PVOID HeapPtr;

enum {
  kHeapExpandable = 2,
  kHeapNoExecute = 4,
  kHeapShared = 6,
  kHeapReadOnly = 8,
  kHeapNoFlags = 0
};

class Heap final {
 private:
  explicit Heap();

 public:
  ~Heap();

 public:
  HCORE_COPY_DEFAULT(Heap);

 public:
  static Heap *Shared() noexcept;

 public:
  void Delete(HeapPtr me) noexcept;
  SizeT Size(HeapPtr me) noexcept;
  HeapPtr New(const SizeT &sz, const Int32 flags = kHeapNoFlags);
};
}  // namespace System