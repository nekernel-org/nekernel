/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved..

	File: IPC.h.
	Purpose: IPC protocol.

------------------------------------------- */

#ifndef INC_IPC_H
#define INC_IPC_H

#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <Hints/CompilerHint.h>

/// @file IPC.h
/// @brief IPC comm. protocol.

/// IA separator.
#define kIPCRemoteSeparator ":"

/// Interchange address, consists of PID:TEAM.
#define kIPCRemoteInvalid "00:00"

#define kIPCHeaderMagic (0x4950434)

namespace Kernel
{
	struct IPC_ADDR;
	struct IPC_MSG;

	/// @brief 128-bit IPC address.
	struct PACKED IPC_ADDR final
	{
		UInt64 UserProcessID;
		UInt64 UserProcessTeam;

		////////////////////////////////////
		// some operators.
		////////////////////////////////////

		bool operator==(const IPC_ADDR& addr) noexcept
		{
			return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
		}

		bool operator==(IPC_ADDR& addr) noexcept
		{
			return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
		}
	};

	typedef struct IPC_ADDR IPC_ADDR;

	enum
	{
		kIPCLittleEndian = 0,
		kIPCBigEndian	 = 1,
		kIPCMixedEndian	 = 2,
	};

	constexpr inline auto kIPCMsgSize = 6094U;

	/// @brief IPC connection header, message cannot be greater than 6K.
	typedef struct IPC_MSG final
	{
		UInt32	 IpcHeaderMagic; // cRemoteHeaderMagic
		UInt8	 IpcEndianess;	 // 0 : LE, 1 : BE
		SizeT	 IpcPacketSize;
		IPC_ADDR IpcFrom;
		IPC_ADDR IpcTo;
		UInt32	 IpcCRC32;
		UInt32	 IpcMsg;
		UInt32	 IpcMsgSz;
		UInt8	 IpcData[kIPCMsgSize];

		/// @brief Passes the message to target, could be anything, HTTP packet, JSON or whatever.
		Bool Pass(IPC_MSG* target) noexcept
		{
			if (target && target->IpcFrom == this->IpcTo)
			{
				if (this->IpcMsgSz > target->IpcMsgSz)
					return No;

				rt_copy_memory(this->IpcData, target->IpcData, this->IpcMsgSz);

				return Yes;
			}

			return No;
		}
	} PACKED IPC_MSG;

	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(_Input IPC_MSG* pckt_in);

	/// @brief Construct packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_construct_packet(_Output _Input IPC_MSG** pckt_in);
} // namespace Kernel

#endif // INC_IPC_H
