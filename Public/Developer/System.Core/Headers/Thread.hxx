/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD_API__
#define __THREAD_API__

#include <System.Core/Headers/Defines.hxx>

#define kThreadErrorExit -33

/// @brief Thread Information Block, which holds information about the running
/// thread.
typedef QWordType ThreadRef;

/// @brief Main application thread.
CA_EXTERN_C ThreadRef kMainThread;

typedef VoidType(*ThreadEntrypointKind)(VoidType);

/// @brief Creates a new thread, and runs the code.
/// @param threadName the thread's name.
/// @param threadStart where to start.
/// @return 
CA_EXTERN_C ThreadRef CreateThread(const CharacterTypeUTF8* threadName, ThreadEntrypointKind threadStart);

/// @brief Dispoes the thread, and exits with code kThreadErrorExit
/// @param ref 
/// @return 
CA_EXTERN_C VoidType  DisposeThread(ThreadRef ref);

#endif  // __THREAD_API__
