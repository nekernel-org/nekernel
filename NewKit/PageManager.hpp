// a way to create and find our pages.
// I'm thinking about a separate way of getting a paged area.

/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/PageAllocator.hpp>
#include <NewKit/Ref.hpp>

#ifndef kBadAddress
#define kBadAddress (0)
#endif // #ifndef kBadAddress

namespace hCore
{
class PageManager;

class PTEWrapper final
{
  public:
    explicit PTEWrapper(Boolean Rw = false, Boolean User = false, Boolean ExecDisable = false, UIntPtr Address = 0);
    ~PTEWrapper();

    PTEWrapper &operator=(const PTEWrapper &) = default;
    PTEWrapper(const PTEWrapper &) = default;

  public:
    void FlushTLB(Ref<PageManager> &pm);
    const UIntPtr &VirtualAddress();

    bool Reclaim();
    bool Shareable();
    bool Present();
    bool Access();

  private:
    Boolean m_Rw;
    Boolean m_User;
    Boolean m_ExecDisable;
    UIntPtr m_VirtAddr;
    Boolean m_Cache;
    Boolean m_Shareable;
    Boolean m_Wt;
    Boolean m_Present;
    Boolean m_Accessed;

  private:
    friend class PageManager;
    friend class Pmm;
};

struct PageManager final
{
  public:
    PageManager() = default;
    ~PageManager() = default;

    PageManager &operator=(const PageManager &) = default;
    PageManager(const PageManager &) = default;

  public:
    PTEWrapper *Request(Boolean Rw, Boolean User, Boolean ExecDisable);
    bool Free(Ref<PTEWrapper*> &wrapper);

  private:
    void FlushTLB(UIntPtr VirtAddr);

  private:
    friend PTEWrapper;
    friend class Pmm;
};
} // namespace hCore
