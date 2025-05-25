/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

namespace ZXD {
using namespace Kernel;

struct ZXD_EXEC_HEADER;
struct ZXD_STUB_HEADER;

/// @brief ZXD executable header
/// @details This header is used to identify ZXD executable files.
struct ZXD_EXEC_HEADER {
  UInt32  fMagic;
  UInt32  fVersion;
  UInt32  fFlags;
  UInt32  fHdrSize;
  UInt32  fCRC32;
  UInt32  fAssigneeSignature;
  UInt32  fIssuerSingature;
  UIntPtr fExecOffset;
  SizeT   fExecSize;
  UIntPtr fStubOffset;
  SizeT   fStubSize;
  SizeT   fStubAlign;
  SizeT   fStubCount;
};

/// @brief ZXD stub header
/// @details This header is used to identify ZXD stub files. It contains the size of the stub, the
/// offset of the stub, and the CRC32 checksum of the stub.
struct ZXD_STUB_HEADER {
  UInt32 fStubSize;
  UInt32 fStubOffset;
  UInt32 fStubCRC32;
};
}  // namespace ZXD