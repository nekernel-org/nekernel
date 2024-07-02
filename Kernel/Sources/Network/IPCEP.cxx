/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <NetworkKit/IPCEP.hxx>
#include <KernelKit/HError.hpp>
#include <KernelKit/ProcessScheduler.hxx>

using namespace Kernel;

/// @internal
/// @brief The internal sanitize function.
Bool __ipc_sanitize_packet(IPCEPMessageHeader* pckt)
{
	if (!pckt) return false;

	auto endian = DEDUCE_ENDIAN(pckt, ((char*)pckt)[0]);

	switch (endian)
	{
	case Endian::kEndianBig:
	{
		if (pckt->IpcEndianess == eIPCEPLittleEndian)
			goto _Fail;

		break;
	}
	case Endian::kEndianLittle:
	{
		if (pckt->IpcEndianess == eIPCEPBigEndian)
			goto _Fail;

		break;
	}
	case Endian::kEndianMixed:
		break;
	default:
		goto _Fail;
	}

	if (pckt->IpcFrom == pckt->IpcTo)
	{
		goto _Fail;
	}
	if (pckt->IpcPacketSize > cIPCEPMsgSize)
	{
		goto _Fail;
	}

	return pckt->IpcPacketSize > 1 && pckt->IpcHeaderMagic == cRemoteHeaderMagic;

_Fail:
	ErrLocal() = kErrorIPC;
	return false;
}

namespace Kernel
{
	/// @brief Sanitize packet function
	/// @retval true packet is correct.
	/// @retval false packet is incorrect and process has crashed.
	Bool ipc_sanitize_packet(IPCEPMessageHeader* pckt)
	{
		if (!__ipc_sanitize_packet(pckt))
		{
			ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
			return false;
		}

		return true;
	}
}
