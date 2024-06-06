/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD__
#define __THREAD__

#include <Headers/Defines.h>

#define kThreadErrorExit (-33)

/// @brief Thread reference.
typedef QWordType ThreadRef;

/// @brief Main application thread.
CS_EXTERN_C ThreadRef kMainThread;

typedef VoidType (*ThreadEntrypointKind)(VoidType);

/// @brief Creates a new thread, and runs the code.
/// @param threadName the thread's name.
/// @param threadStart where to start.
/// @return
CS_EXTERN_C ThreadRef CSThreadCreate(const CharacterTypeUTF8* threadName, ThreadEntrypointKind threadStart);

/// @brief Dispoes the thread, and exits with code kThreadErrorExit
/// @param ref the thread reference.
/// @return  nothing.
CS_EXTERN_C VoidType CSThreadRelease(ThreadRef ref);

/// @brief Waits for the thread to complete.
/// @param ref the thread reference.
/// @return  nothing.
CS_EXTERN_C VoidType CSThreadJoin(ThreadRef ref);

/// @brief Yields the current thread.
/// @param ref the thead reference.
/// @return
CS_EXTERN_C VoidType CSThreadYield(ThreadRef ref);

#endif // __THREAD__
