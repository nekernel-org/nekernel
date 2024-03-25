/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD_API__
#define __THREAD_API__

#include <System.Core/Headers/Defines.hxx>

/// @brief Thread Information Block, which holds information about the running
/// thread.
typedef QWordType ThreadRef;

/// @brief Main application thread.
CA_EXTERN_C ThreadRef kMainThread;

#endif  // __THREAD_API__
