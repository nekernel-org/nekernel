/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

/// @brief SOM class, translated to C++

using namespace HCore;

namespace System {
class MemoryException;

typedef VoidPtr HeapPtr;

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

class MemoryException final {
 public:
  explicit MemoryException() = default;
  ~MemoryException() = default;

 public:
  HCORE_COPY_DEFAULT(MemoryException);

 public:
  const char *Name();
  const char *Reason();

 private:
  const char *mReason{"HeapAPI: Memory Exception!"};

 private:
  friend Heap;
};
} // namespace System