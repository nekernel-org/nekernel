/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/Defines.hpp>

using namespace HCore;

class HMemoryException;

typedef VoidPtr HHeapPtr;

enum {
  kHeapExpandable = 2,
  kHeapNoExecute = 4,
  kHeapShared = 6,
  kHeapReadOnly = 8,
  kHeapNoFlags = 0
};

class HHeap final {
 private:
  explicit HHeap();

 public:
  ~HHeap();

 public:
  HCORE_COPY_DEFAULT(HHeap);

 public:
  static HHeap *Shared() noexcept;

 public:
  void Delete(HHeapPtr me) noexcept;
  SizeT Size(HHeapPtr me) noexcept;
  HHeapPtr New(const SizeT &sz, const Int32 flags = kHeapNoFlags);
};

class HMemoryException final {
 public:
  explicit HMemoryException() = default;
  ~HMemoryException() = default;

 public:
  HCORE_COPY_DEFAULT(HMemoryException);

 public:
  const char *Name();
  const char *Reason();

 private:
  const char *mReason{"HeapAPI: Memory Exception!"};

 private:
  friend HHeap;
};
