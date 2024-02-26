/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

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

namespace HCore {
template <SSizeT ID>
class SystemCall {
 public:
  explicit SystemCall() { kcout << "SystemCall::SystemCall"; }

  virtual ~SystemCall() { kcout << "SystemCall::~SystemCall"; }

  SystemCall &operator=(const SystemCall &) = default;
  SystemCall(const SystemCall &) = default;

  // Should not be called alone!
  virtual bool Exec() const {
    kcout << "SystemCall->Exec<RET>()";
    return false;
  }
};

constexpr static inline SSizeT syscall_hash(const char *seed, int mul) {
  SSizeT hash = 0;

  for (SSizeT idx = 0; seed[idx] != 0; ++idx) {
    hash += seed[idx];
    hash ^= mul;
  }

  return hash;
}

bool ke_init_hal();
}  // namespace HCore

#define kKernelMaxSystemCalls (256)

extern HCore::Array<void (*)(HCore::Int32 id, HCore::HAL::StackFrame *),
                    kKernelMaxSystemCalls>
    kSyscalls;

EXTERN_C HCore::Void rt_wait_for_io();
EXTERN_C HCore::Void rt_syscall_handle(HCore::HAL::StackFramePtr stackFrame);
EXTERN_C HCore::HAL::StackFramePtr rt_get_current_context();
EXTERN_C HCore::Void rt_do_context_switch(HCore::HAL::StackFramePtr stackFrame);

inline HCore::VoidPtr kKernelVirtualStart;
inline HCore::UIntPtr kKernelVirtualSize;

inline HCore::VoidPtr kKernelPhysicalStart;
inline HCore::UIntPtr kKernelPhysicalSize;
