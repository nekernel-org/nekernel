/* -------------------------------------------

	Copyright Zeta Electronics Corporation.

	File: IPCEP.hxx.
	Purpose: IPC protocol.

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_HXX_
#define _INC_IPC_ENDPOINT_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief IPC Endpoint Protocol (IPCEP for short).

/// IA separator.
#define cRemoteSeparator "."

/// Interchange address, consists of domain:namespace.
#define cRemoteInvalid	"00.00.00.00:0000"
#define cRemoteBitWidth (96) /* 96-bit address space. */

#define cRemoteHeaderMagic (0x4950434550)

namespace NewOS
{
	/// @brief 96-bit number to represent the domain and namespace
	struct PACKED IPCEPAddress
	{
		UInt32 RemoteAddress;
		UInt64 RemoteNamespace;
	};

	typedef struct IPCEPAddress IPCEPAddressType;

	enum
	{
		eIPCEPLittleEndian = 0,
		eIPCEPBigEndian	   = 1
	};

	/// @brief IPCEP connection header
	typedef struct IPCEPConnectionHeader
	{
		UInt32			 IpcHeader;	   // cRemoteHeaderMagic
		UInt8			 IpcEndianess; // 0 : LE, 1 : BE
		SizeT			 IpcPacketSize;
		IPCEPAddressType IpcFrom;
		IPCEPAddressType IpcTo;
		UInt32			 IpcCRC32;
		SizeT			 IpcDataSize;
		Char			 IpcData[];
	} PACKED IPCEPConnectionHeader;
} // namespace NewOS

#endif // _INC_IPC_ENDPOINT_HXX_
