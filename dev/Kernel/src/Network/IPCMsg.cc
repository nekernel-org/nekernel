/* -------------------------------------------

	Copyright (C) 2024, t& Labs, all rights reserved.

------------------------------------------- */

#include <NetworkKit/IPC.h>
#include <KernelKit/LPC.h>
#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/// @internal
	/// @brief The internal sanitize function.
	Bool ipc_int_sanitize_packet(IPC_MSG* pckt)
	{
		auto endian = rtl_deduce_endianess(pckt, ((Char*)pckt)[0]);

		switch (endian)
		{
		case Endian::kEndianBig: {
			if (pckt->IpcEndianess == kIPCLittleEndian)
				goto ipc_check_failed;

			break;
		}
		case Endian::kEndianLittle: {
			if (pckt->IpcEndianess == kIPCBigEndian)
				goto ipc_check_failed;

			break;
		}
		case Endian::kEndianMixed: {
			if (pckt->IpcEndianess == kIPCMixedEndian)
				goto ipc_check_failed;

			break;
		}
		default:
			goto ipc_check_failed;
		}

		if (pckt->IpcFrom == pckt->IpcTo ||
			pckt->IpcPacketSize > kIPCMsgSize)
		{
			goto ipc_check_failed;
		}

		return pckt->IpcPacketSize > 1 && pckt->IpcHeaderMagic == kIPCHeaderMagic;

	ipc_check_failed:
		err_local_get() = kErrorIPC;
		return false;
	}

	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(IPC_MSG* pckt)
	{
		if (!pckt ||
			!ipc_int_sanitize_packet(pckt))
		{
			UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
			return false;
		}

		return true;
	}

	/// @brief Construct packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_construct_packet(_Output IPC_MSG** pckt_in)
	{
		// don't act if it's not even valid.
		if (!pckt_in)
			return false;

		// don't do anything if it's valid already.
		if (*pckt_in)
			return true;

		// crash process if the packet pointer of pointer is NULL.
		if (!pckt_in)
		{
			UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
			return false;
		}

		*pckt_in = new IPC_MSG();

		if (*pckt_in)
		{
			auto endian = rtl_deduce_endianess((*pckt_in), ((Char*)(*pckt_in))[0]);

			(*pckt_in)->IpcHeaderMagic = kIPCHeaderMagic;

			(*pckt_in)->IpcEndianess  = static_cast<UInt8>(endian);
			(*pckt_in)->IpcPacketSize = sizeof(IPC_MSG);

			(*pckt_in)->IpcTo.UserProcessID	  = 0;
			(*pckt_in)->IpcTo.UserProcessTeam = 0;

			(*pckt_in)->IpcFrom.UserProcessID	= Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().ProcessId;
			(*pckt_in)->IpcFrom.UserProcessTeam = Kernel::UserProcessScheduler::The().CurrentTeam().mTeamId;

			return Yes;
		}

		return No;
	}
} // namespace Kernel
