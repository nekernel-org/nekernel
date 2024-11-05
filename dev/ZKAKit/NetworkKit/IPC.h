/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved..

	File: IPC.h.
	Purpose: IPC protocol.

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_H_
#define _INC_IPC_ENDPOINT_H_

#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <HintKit/CompilerHint.h>
#include <CompressKit/RLE.h>

/// @file IPC.h
/// @brief IPC EP protocol.

/// IA separator.
#define cXPCOMRemoteSeparator ":"

/// Interchange address, consists of PID:TEAM.
#define cXPCOMRemoteInvalid "00:00"

#define cXPCOMHeaderMagic (0x4950434)

namespace Kernel
{
	struct IPC_ADDRESS_STRUCT;
	struct IPC_MESSAGE_STRUCT;

	/// @brief 128-bit IPC address.
	struct PACKED IPC_ADDRESS_STRUCT final
	{
		UInt64 UserProcessID;
		UInt64 UserProcessTeam;

		////////////////////////////////////
		// some operators.
		////////////////////////////////////

		bool operator==(const IPC_ADDRESS_STRUCT& addr) noexcept
		{
			return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
		}

		bool operator==(IPC_ADDRESS_STRUCT& addr) noexcept
		{
			return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
		}
	};

	typedef struct IPC_ADDRESS_STRUCT IPCEPAddressKind;

	enum
	{
		eIPCEPLittleEndian = 0,
		eIPCEPBigEndian	   = 1,
		eIPCEPMixedEndian  = 2,
	};

	constexpr auto cXPCOMMsgSize = 6094U;

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
		UInt8			 IpcData[cXPCOMMsgSize];
	} PACKED IPC_MESSAGE_STRUCT;

	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(_Input IPC_MESSAGE_STRUCT* pckt_in);

	/// @brief Construct packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_construct_packet(_Output IPC_MESSAGE_STRUCT** pckt_in);
} // namespace Kernel

#endif // _INC_IPC_ENDPOINT_H_
