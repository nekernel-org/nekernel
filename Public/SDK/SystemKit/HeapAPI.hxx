/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/Compiler.hpp>
#include <NewKit/Defines.hpp>

using namespace hCore;

class MeMemoryException;

typedef void *MeHeapPtr;

enum
{
    kHeapExpandable = 2,
    kHeapNoExecute = 4,
    kHeapShared = 6,
    kHeapReadOnly = 8,
    kHeapNoFlags = 0
};

class MeHeap final
{
  private:
    explicit MeHeap();

  public:
    ~MeHeap();

  public:
    HCORE_COPY_DEFAULT(MeHeap);

  public:
    static MeHeap *Shared() noexcept;

  public:
    void Dispose(MeHeapPtr me) noexcept;
    SizeT Tell(MeHeapPtr me) noexcept;
    MeHeapPtr New(const SizeT &sz, const Int32 flags = kHeapNoFlags);
};

class MeMemoryException final
{
  public:
    MeMemoryException() = default;
    ~MeMemoryException() = default;

  public:
    HCORE_COPY_DEFAULT(MeMemoryException);

  public:
    const char *Name();
    const char *Reason();

  private:
    const char *mReason{"Memory error!"};

  private:
    friend MeHeap;
};
