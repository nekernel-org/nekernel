/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <EFIKit/EFI.hxx>

namespace hCore::HEL {
/**
    @brief the kind of executable we're loading.
*/
enum {
  kTypeKernel = 100,
  kTypeKernelDriver = 101,
  kTypeRsrc = 102,
  kTypeCount = 3,
};

/**
    @brief The executable architecture.
*/

enum {
  kArchAmd64 = 122,
  kArchCount = 2,
};

struct __attribute__((packed)) HandoverHeader final {
  Int32 targetMagic;
  Int32 targetType;
  Int32 targetArch;
  UIntPtr startAddress;
};
}  // namespace hCore::HEL

#define kHandoverMagic 0xBAD55

#define kBaseHandoverStruct 0x80000000
#define kHandoverStructSz sizeof(HEL::HandoverHeader)
