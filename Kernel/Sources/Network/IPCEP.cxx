/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <NetworkKit/IPCEP.hxx>

using namespace NewOS;

Bool ipc_sanitize_packet(IPCEPMessageHeader* pckt)
{
	if (!pckt) return false;

	auto endian = DEDUCE_ENDIAN(pckt, ((char*)pckt)[0]);

	switch (endian)
	{
	case Endian::kEndianBig:
	{
		if (pckt->IpcEndianess == eIPCEPLittleEndian)
			return false;

		break;
	}
	case Endian::kEndianLittle:
	{
		if (pckt->IpcEndianess == eIPCEPBigEndian)
			return false;

		break;
	}
	case Endian::kEndianMixed:
		break;
	default:
		return false;
	}

	if (pckt->IpcFrom == pckt->IpcTo) return false;
	if (pckt->IpcPacketSize > cIPCEPMsgSize) return false;

	return pckt->IpcPacketSize > 1 && pckt->IpcHeaderMagic == cRemoteHeaderMagic;
}
