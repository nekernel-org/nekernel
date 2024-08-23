/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NetworkKit/IPC.hxx>
#include <KernelKit/LPC.hxx>
#include <KernelKit/ProcessScheduler.hxx>

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
	case Endian::kEndianMixed:
		break;
	default:
		goto ipc_check_failed;
	}

	if (pckt->IpcFrom == pckt->IpcTo ||
		pckt->IpcPacketSize > cIPCEPMsgSize)
	{
		goto ipc_check_failed;
	}

	return pckt->IpcPacketSize > 1 && pckt->IpcHeaderMagic == cRemoteHeaderMagic;

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
			ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
			return false;
		}

		return true;
	}
} // namespace Kernel
