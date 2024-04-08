/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Function.hpp>

#ifdef __NEWOS_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <HALKit/AMD64/Hypervisor.hpp>
#include <HALKit/AMD64/Processor.hpp>
#elif defined(__NEWOS_PPC__)
#include <HALKit/PowerPC/Processor.hpp>
#else
#error Unknown architecture
#endif

namespace NewOS {
constexpr static inline SSizeT rt_hash_seed(const char *seed, int mul) {
  SSizeT hash = 0;

  for (SSizeT idx = 0; seed[idx] != 0; ++idx) {
    hash += seed[idx];
    hash ^= mul;
  }

  return hash;
}
}  // namespace NewOS

#define kKernelMaxSystemCalls (256)

typedef NewOS::Void (*rt_syscall_proc)(NewOS::HAL::StackFramePtr);

extern NewOS::Array<rt_syscall_proc,
                    kKernelMaxSystemCalls>
    kSyscalls;

EXTERN_C NewOS::Void rt_wait_400ns();
EXTERN_C NewOS::HAL::StackFramePtr rt_get_current_context();
EXTERN_C NewOS::Void rt_do_context_switch(NewOS::HAL::StackFramePtr stackFrame);

inline NewOS::VoidPtr kKernelVirtualStart;
inline NewOS::UIntPtr kKernelVirtualSize;

inline NewOS::VoidPtr kKernelPhysicalStart;
inline NewOS::UIntPtr kKernelPhysicalSize;

#include <FirmwareKit/Handover.hxx>

inline NewOS::HEL::HandoverInformationHeader* kHandoverHeader;
