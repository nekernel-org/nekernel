/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/**
 * @file Handover.hxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief The handover boot protocol.
 * @version 0.2
 * @date 2024-02-23
 *
 * @copyright Copyright (c) 2024, Mahrouss Logic
 *
 */

#pragma once

#include <NewKit/Defines.hpp>

/* useful macros */

#define kHandoverMagic 0xBADCC

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
typedef struct HandoverHeader final {
  UInt64 f_TargetMagic;
  Int32 f_TargetType;
  Int32 f_TargetArch;
  UIntPtr f_TargetStartAddress;
} __attribute__((packed)) HandoverHeader, *HandoverHeaderPtr;

struct HandoverInformationHeader {
  UInt64 f_Magic;
  UInt64 f_Version;
  UInt64 f_Bootloader;

  voidPtr f_VirtualStart;
  SizeT f_VirtualSize;
  voidPtr f_PhysicalStart;
  SizeT f_PhysicalSize;
  WideChar f_FirmwareVendorName[32];
  SizeT f_FirmwareVendorLen;
  struct {
    VoidPtr f_VendorTables;
  } f_HardwareTables;
  struct {
    UIntPtr f_The;
    SizeT f_Size;
    UInt32 f_Width;
    UInt32 f_Height;
    UInt32 f_PixelFormat;
    UInt32 f_PixelPerLine;
  } f_GOP;
};

/**
    @brief Handover Jump Proc
*/
typedef void (*HandoverProc)(HandoverInformationHeader* pHandover);
}  // namespace HCore::HEL
