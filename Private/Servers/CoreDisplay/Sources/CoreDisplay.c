/* -------------------------------------------

    Copyright Mahrouss Logic

    File: CoreDisplay.c
    Purpose: Display server.

------------------------------------------- */

#include <CoreDisplay.h>
#include <IPC.h>

/// @brief Called when the server starts.
DWordType ServerStartup(VoidType)
{
	CDInitDisplay(kCDNoFlags); // init standard display. Need to notify other endpoits.
			  // as well.
			  //
	
	IPCSendMessage(kIPCBroadcastNew); /// broadcast our presence 

	return 0;
}

/// @brief Called when the server shuts down.
DWordType ServerShutdown(VoidType)
{
	CDCloseDisplay(kCDShutdownAll); /// takes the close flag argument, which tells here to also close the child nodes.
	IPCSendMessage(kIPCBroadcastDispose); /// broadcast our presence 
	return 0;
}

/// EOF.
