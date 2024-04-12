/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Server.c
    Purpose: DisplayManager server.

------------------------------------------- */

#include "Server.h"

/// @brief Called when the server starts.
DWordType ServerStartup(VoidType)
{
	DMInitDisplay(kDMNoFlags); // init standard display. Need to notify other endpoits.
			  // as well.
			  //
	
	IPCSendMessage(kIPCBroadcast); /// broadcast our presence 

	return 0;
}

/// @brief Called when the server shuts down.
DWordType ServerShutdown(VoidType)
{
	DMCloseDisplay(); /// takes no arguments.
	return 0;
}

