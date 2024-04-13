/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Server.c
    Purpose: Display server.

------------------------------------------- */

#include <CoreDisplay.h>
#include <IPCWrapper.h>

/// @brief Called when the server starts.
DWordType ServerStartup(VoidType)
{
	CDInitDisplay(kDMNoFlags); // init standard display. Need to notify other endpoits.
			  // as well.
			  //
	
	IPCSendMessage(kIPCBroadcast); /// broadcast our presence 

	return 0;
}

/// @brief Called when the server shuts down.
DWordType ServerShutdown(VoidType)
{
	CDCloseDisplay(); /// takes no arguments.
	return 0;
}

/// EOF.
