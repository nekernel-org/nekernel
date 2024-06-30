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

/// Interchange address, consists of PID:TEAM.
#define cRemoteInvalid	"00:00"

#define cRemoteHeaderMagic (0x4950434)

namespace NewOS
{
	/// @brief 128-bit IPC address.
	struct PACKED IPCEPAddress final
	{
		UInt64 ProcessID;
		UInt64 ProcessTeam;

		////////////////////////////////////
		// some operators.
		////////////////////////////////////

		bool operator==(const IPCEPAddress& addr) noexcept
		{
			return addr.ProcessID == this->ProcessID && addr.ProcessTeam == this->ProcessTeam;
		}

		bool operator==(IPCEPAddress& addr) noexcept
		{
			return addr.ProcessID == this->ProcessID && addr.ProcessTeam == this->ProcessTeam;
		}
	};

	typedef struct IPCEPAddress IPCEPAddressType;

	enum
	{
		eIPCEPLittleEndian = 0,
		eIPCEPBigEndian	   = 1
	};

	constexpr auto cIPCEPMsgSize = 6094U;

	/// @brief IPCEP connection header, message cannot be greater than 6K.
	typedef struct IPCEPMessageHeader final
	{
		UInt32			 IpcHeaderMagic;	   // cRemoteHeaderMagic
		UInt8			 IpcEndianess; // 0 : LE, 1 : BE
		SizeT			 IpcPacketSize;
		IPCEPAddressType IpcFrom;
		IPCEPAddressType IpcTo;
		UInt32			 IpcCRC32;
		UInt32			 IpcMsg;
		UInt32			 IpcMsgSz;
		UInt8			 IpcData[cIPCEPMsgSize];
	} PACKED IPCEPMessageHeader;

	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(IPCEPMessageHeader* pckt);
} // namespace NewOS

#endif // _INC_IPC_ENDPOINT_HXX_
