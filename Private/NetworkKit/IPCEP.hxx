/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_HXX_
#define _INC_IPC_ENDPOINT_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief IPC Endpoint Protocol (IPCEP) definition.

/// IA separator.
#define kRemoteSeparator "."

/// Interchange address, consists of domain+namespace.
#define kRemoteInvalid "00.00.00.00:0"
#define kRemoteMaxLen 21

#define kRemoteHeaderMagic 0xFFEEAACCEE

namespace NewOS {
/// @brief 96-bit number to represent the domain and namespace
struct PACKED IPCEPNumber final {
  UInt32 RemoteAddress;
  UInt64 RemoteNamespace;
};

typedef struct IPCEPNumber IPCEPNumberType;

/// @brief IPCEP connection header
typedef struct IPCEPConnectionHeader final {
  UInt64 IpcHeader;    // 0xFFEEAACCEE
  UInt8 IpcEndianess;  // 0 : LE, 1 : BE
  SizeT IpcPacketSize;
  IPCEPNumberType IpcFrom;
  IPCEPNumberType IpcTo;
  UInt32 IpcCRC32;
  Char IpcPad[8];
} PACKED IPCEPConnectionHeader;
}  // namespace NewOS

#endif  // _INC_IPC_ENDPOINT_HXX_