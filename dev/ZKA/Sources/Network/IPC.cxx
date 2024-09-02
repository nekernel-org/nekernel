/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NetworkKit/IPC.hxx>
#include <KernelKit/LPC.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

using namespace Kernel;

/// @internal
/// @brief The internal sanitize function.
Bool ipc_int_sanitize_packet(IPC_MESSAGE_STRUCT* pckt)
{
	auto endian = DEDUCE_ENDIAN(pckt, ((Char*)pckt)[0]);

	switch (endian)
	{
	case Endian::kEndianBig: {
		if (pckt->IpcEndianess == eIPCEPLittleEndian)
			goto ipc_check_failed;

		break;
	}
	case Endian::kEndianLittle: {
		if (pckt->IpcEndianess == eIPCEPBigEndian)
			goto ipc_check_failed;

		break;
	}
	case Endian::kEndianMixed: {
		if (pckt->IpcEndianess == eIPCEPMixedEndian)
			goto ipc_check_failed;

		break;
	}
	default:
		goto ipc_check_failed;
	}

	if (pckt->IpcFrom == pckt->IpcTo ||
		pckt->IpcPacketSize > cXPCOMMsgSize)
	{
		goto ipc_check_failed;
	}

	return pckt->IpcPacketSize > 1 && pckt->IpcHeaderMagic == cXPCOMHeaderMagic;

ipc_check_failed:
	ErrLocal() = kErrorIPC;
	return false;
}

namespace Kernel
{
	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(IPC_MESSAGE_STRUCT* pckt)
	{
		if (!pckt ||
			!ipc_int_sanitize_packet(pckt))
		{
			UserProcessScheduler::The().CurrentProcess().Leak().Crash();
			return false;
		}

		return true;
	}

	/// @brief Construct packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_construct_packet(_Output IPC_MESSAGE_STRUCT** pckt_in)
	{
		// don't do anything if it's valid already.
		if (*pckt_in)
			return true;

		// crash process if the packet pointer of pointer is NULL.
		if (!pckt_in)
		{
			UserProcessScheduler::The().CurrentProcess().Leak().Crash();
			return false;
		}

		*pckt_in = new IPC_MESSAGE_STRUCT();

		MUST_PASS((*pckt_in));

		if (*pckt_in)
		{
			auto endian = DEDUCE_ENDIAN((*pckt_in), ((Char*)(*pckt_in))[0]);

			(*pckt_in)->IpcHeaderMagic = cXPCOMHeaderMagic;

			(*pckt_in)->IpcEndianess  = static_cast<UInt8>(endian);
			(*pckt_in)->IpcPacketSize = sizeof(IPC_MESSAGE_STRUCT);

			(*pckt_in)->IpcTo.UserProcessID	  = 0;
			(*pckt_in)->IpcTo.UserProcessTeam = 0;

			(*pckt_in)->IpcFrom.UserProcessID	= Kernel::UserProcessScheduler::The().CurrentProcess().Leak().ProcessId;
			(*pckt_in)->IpcFrom.UserProcessTeam = Kernel::UserProcessScheduler::The().CurrentTeam().mTeamId;

			return true;
		}

		UserProcessScheduler::The().CurrentProcess().Leak().Crash();
		return false;
	}
} // namespace Kernel
