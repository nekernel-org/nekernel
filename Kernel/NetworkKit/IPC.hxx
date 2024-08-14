/* -------------------------------------------

	Copyright ZKA Technologies..

	File: IPC.hxx.
	Purpose: IPC protocol.

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_HXX_
#define _INC_IPC_ENDPOINT_HXX_

#include <NewKit/Defines.hxx>
#include <NewKit/String.hxx>

/// @file IPC.hxx
/// @brief IPC protocol.

/// IA separator.
#define cRemoteSeparator "."

/// Interchange address, consists of PID:TEAM.
#define cRemoteInvalid "00:00"

#define cRemoteHeaderMagic (0x4950434)

namespace Kernel
{
	/// @brief 128-bit IPC address.
	struct PACKED IPC_ADDRESS_STRUCT final
	{
		UInt64 ProcessID;
		UInt64 ProcessTeam;

		////////////////////////////////////
		// some operators.
		////////////////////////////////////

		bool operator==(const IPC_ADDRESS_STRUCT& addr) noexcept
		{
			return addr.ProcessID == this->ProcessID && addr.ProcessTeam == this->ProcessTeam;
		}

		bool operator==(IPC_ADDRESS_STRUCT& addr) noexcept
		{
			return addr.ProcessID == this->ProcessID && addr.ProcessTeam == this->ProcessTeam;
		}
	};

	typedef struct IPC_ADDRESS_STRUCT IPCEPAddressKind;

	enum
	{
		eIPCEPLittleEndian = 0,
		eIPCEPBigEndian	   = 1
	};

	constexpr auto cIPCEPMsgSize = 6094U;

	/// @brief IPC connection header, message cannot be greater than 6K.
	typedef struct IPC_MESSAGE_STRUCT final
	{
		UInt32			 IpcHeaderMagic; // cRemoteHeaderMagic
		UInt8			 IpcEndianess;	 // 0 : LE, 1 : BE
		SizeT			 IpcPacketSize;
		IPCEPAddressKind IpcFrom;
		IPCEPAddressKind IpcTo;
		UInt32			 IpcCRC32;
		UInt32			 IpcMsg;
		UInt32			 IpcMsgSz;
		UInt8			 IpcData[cIPCEPMsgSize];
	} PACKED IPC_MESSAGE_STRUCT;

	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(IPC_MESSAGE_STRUCT* pckt);
} // namespace Kernel

#endif // _INC_IPC_ENDPOINT_HXX_
