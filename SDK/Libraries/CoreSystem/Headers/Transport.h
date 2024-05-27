/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

//
// Created by Amlal on 5/12/24
//

#ifndef __TRANSPORT__
#define __TRANSPORT__

/// @file: Transport.h
/// @brief Open Transport Layer, an alternative to berkeley sockets.

#include <Headers/Defines.h>

typedef QWordType TrStreamType;

/// @brief Opens a new socket
/// @param afType address family
/// @param sockType type of socket
/// @param sockProto socket protocol.
/// @return The STREAMS socket.
/// @note return is const.
CS_EXTERN_C CS_CONST TrStreamType CSOpenSocket(UInt32Type afType, UInt32Type sockType, UInt32Type sockProto);

/// @brief Close a STREAMS socket.
/// @param streams The streams socket.
/// @return 
CS_EXTERN_C VoidType CSCloseSocket(CS_CONST TrStreamType streams);

/// @brief Get OpenTransport version.
/// @param void
/// @return 
CS_EXTERN_C CS_CONST Int32Type CSGetVersion(VoidType);

enum
{
	TrSocketProtoTCP, /// TCP socket
	TrSocketProtoUDP, /// UDP socket
	TrSocketProtoUN, /// IPC  socket
	TrSocketProtoRaw, /// Raw socket
	TrSocketProtoCount,
};

#endif // __TRANSPORT__