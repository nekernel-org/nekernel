/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD__
#define __THREAD__

#include <Headers/Defines.h>

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
CA_EXTERN_C ThreadRef TmCreateThread(const CharacterTypeUTF8* threadName, ThreadEntrypointKind threadStart);

/// @brief Dispoes the thread, and exits with code kThreadErrorExit
/// @param ref the thread reference.
/// @return  nothing.
CA_EXTERN_C VoidType TmDisposeThread(ThreadRef ref);

/// @brief Waits for the thread to complete.
/// @param ref the thread reference.
/// @return  nothing.
CA_EXTERN_C VoidType TmWaitForCompletion(ThreadRef ref);

#endif  // __THREAD__
