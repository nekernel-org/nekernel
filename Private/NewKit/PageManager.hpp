// a way to create and find our pages.
// I'm thinking about a separate way of getting a paged area.

/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/PageAllocator.hpp>
#include <NewKit/Ref.hpp>

#ifndef kBadAddress
#define kBadAddress (0)
#endif  // #ifndef kBadAddress

namespace NewOS {
class PageManager;

class PTEWrapper final {
 public:
  explicit PTEWrapper(Boolean Rw = false, Boolean User = false,
                      Boolean ExecDisable = false, UIntPtr Address = 0);

  ~PTEWrapper();

  PTEWrapper &operator=(const PTEWrapper &) = default;
  PTEWrapper(const PTEWrapper &) = default;

 public:
  const UIntPtr VirtualAddress();

  void NoExecute(const bool enable = false);
  const bool &NoExecute();

  bool Reclaim();
  bool Shareable();
  bool Present();
  bool Access();

 private:
  Boolean fRw;
  Boolean fUser;
  Boolean fExecDisable;
  UIntPtr fVirtAddr;
  Boolean fCache;
  Boolean fShareable;
  Boolean fWt;
  Boolean fPresent;
  Boolean fAccessed;

 private:
  friend class PageManager;
  friend class Pmm;
};

struct PageManager final {
 public:
  PageManager() = default;
  ~PageManager() = default;

  PageManager &operator=(const PageManager &) = default;
  PageManager(const PageManager &) = default;

 public:
  PTEWrapper Request(Boolean Rw, Boolean User, Boolean ExecDisable);
  bool Free(Ref<PTEWrapper *> &wrapper);

 private:
  void FlushTLB(UIntPtr VirtAddr);

 private:
  friend PTEWrapper;
  friend class Pmm;
};
}  // namespace NewOS
