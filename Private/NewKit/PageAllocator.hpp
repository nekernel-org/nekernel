/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/PageManager.hpp>

namespace HCore {
namespace Detail {
UIntPtr create_page_wrapper(Boolean rw, Boolean user);
void exec_disable(UIntPtr addr);
bool page_disable(UIntPtr addr);
}  // namespace Detail

// TODO: SwapVirtualMemoryDevice class!
}  // namespace HCore
