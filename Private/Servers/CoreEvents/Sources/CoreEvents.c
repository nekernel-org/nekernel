/* -------------------------------------------

    Copyright Mahrouss Logic

    File: CoreEvents.c
    Purpose: Event server.

------------------------------------------- */

#include <CoreEvents.h>
#include <IPC.h>

/// @brief Called when the server starts.
DWordType ServerStartup(VoidType)
{
	IPCSendMessage(kIPCBroadcast); /// broadcast our presence 
	return 0;
}

/// @brief Called when the server shuts down.
DWordType ServerShutdown(VoidType)
{
	IPCSendMessage(kIPCBroadcastDispose); /// broadcast our dispose. 
	return 0;
}

/// EOF.
