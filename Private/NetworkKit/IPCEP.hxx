/* -------------------------------------------

    Copyright Mahrouss Logic.

    File: IPCEP.hxx, Purpose: Common IPC protocol. 

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_HXX_
#define _INC_IPC_ENDPOINT_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief Common IPC Endpoint Protocol (Common IPC for short).

/// IA separator.
#define kRemoteSeparator "."

/// Interchange address, consists of domain:namespace.
#define kRemoteInvalid "00.00.00.00:0000"
#define kRemoteBitWidth 96 /* 96-bit address space. */

#define kRemoteHeaderMagic 0xFEEDFACE

namespace NewOS {
/// @brief 96-bit number to represent the domain and namespace
struct PACKED IPCEPAddress {
  UInt32 RemoteAddress;
  UInt64 RemoteNamespace;
};

typedef struct IPCEPAddress IPCEPAddressType;

enum { kIPCEPLittleEndian = 0, kIPCEPBigEndian = 1 }; 

/// @brief IPCEP connection header
typedef struct IPCEPConnectionHeader {
  UInt32 IpcHeader;    // kRemoteHeaderMagic
  UInt8 IpcEndianess;  // 0 : LE, 1 : BE
  SizeT IpcPacketSize;
  IPCEPAddressType IpcFrom;
  IPCEPAddressType IpcTo;
  UInt32 IpcCRC32;
  SizeT IpcDataSize;
  Char IpcData[];
} PACKED IPCEPConnectionHeader;
}  // namespace NewOS

#endif  // _INC_IPC_ENDPOINT_HXX_
