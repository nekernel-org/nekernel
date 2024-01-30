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

/* useful macros */

#define kHandoverMagic 0xBAD55

#define kBaseHandoverStruct 0x80000000
#define kHandoverStructSz sizeof(HEL::HandoverHeader)

namespace HCore::HEL {
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

/**
@brief The first struct that we read when inspecting The executable
it tells us more about it and IS format independent.
*/
struct __attribute__((packed)) HandoverHeader final {
  Int32 f_TargetMagic;
  Int32 f_TargetType;
  Int32 f_TargetArch;
  UIntPtr f_TargetStartAddress;
};

struct HandoverInformationHeader {
  HandoverHeader* f_Header;
  voidPtr f_VirtualStart;
  SizeT f_VirtualSize;
  voidPtr f_PhysicalStart;
  SizeT f_PhysicalSize;
  Char f_FirmwareVendorName[32];
  SizeT f_FirmwareVendorLen;
  voidPtr f_RsdPtr;
  voidPtr f_SmBIOS;
  voidPtr f_RTC;
};

/**
    @brief Handover Jump Proc
*/
typedef UInt64 (*HandoverProc)(HandoverInformationHeader* pHandover);
}  // namespace HCore::HEL
