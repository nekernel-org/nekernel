/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/**
 * @file Handover.hxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief Handover protocol.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024, Mahrouss Logic
 *
 */

#pragma once

#include <NewKit/Defines.hpp>

/* useful macros */

#define kHandoverMagic 0xBADCC

#define kHandoverStartKernel 0xffffffff10000000
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
  voidPtr f_GOP;
  voidPtr f_GOPSize;
};

/**
    @brief Handover Jump Proc
*/
typedef UInt64 (*HandoverProc)(HandoverInformationHeader* pHandover);
}  // namespace HCore::HEL
