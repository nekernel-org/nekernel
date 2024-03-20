/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <HALKit/AMD64/Hypervisor.hpp>
#include <HALKit/AMD64/Processor.hpp>
#elif defined(__powerpc64__)
#include <HALKit/PowerPC/Processor.hpp>
#else
#error Unknown architecture
#endif

#define kVirtualAddressStartOffset 0x100

namespace HCore {
constexpr static inline SSizeT rt_hash_seed(const char *seed, int mul) {
  SSizeT hash = 0;

  for (SSizeT idx = 0; seed[idx] != 0; ++idx) {
    hash += seed[idx];
    hash ^= mul;
  }

  return hash;
}
}  // namespace HCore

#define kKernelMaxSystemCalls (256)

typedef HCore::Void (*rt_syscall_proc)(HCore::HAL::StackFramePtr);

extern HCore::Array<rt_syscall_proc,
                    kKernelMaxSystemCalls>
    kSyscalls;

EXTERN_C HCore::Void rt_wait_400ns();
EXTERN_C HCore::HAL::StackFramePtr rt_get_current_context();
EXTERN_C HCore::Void rt_do_context_switch(HCore::HAL::StackFramePtr stackFrame);

inline HCore::VoidPtr kKernelVirtualStart;
inline HCore::UIntPtr kKernelVirtualSize;

inline HCore::VoidPtr kKernelPhysicalStart;
inline HCore::UIntPtr kKernelPhysicalSize;

#include <FirmwareKit/Handover.hxx>

inline HCore::HEL::HandoverInformationHeader* kHandoverHeader;
