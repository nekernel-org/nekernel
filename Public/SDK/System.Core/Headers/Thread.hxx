/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD_API__
#define __THREAD_API__

#include <System.Core/Headers/Defs.hxx>

/// @brief Thread Information Block, which holds information about the running thread.
typedef PtrVoidType PtrThread;

/// @brief Creates a new thread.
/// @param StartProc 
/// @param OptionalHeap 
/// @param OptionalStack 
/// @param Detach 
/// @param Join 
/// @return 
PtrThread HcCreateThread(_Input PtrVoidType StartProc, 
              _Optional _InOut PtrVoidType OptionalHeap, 
              _Optional _InOut PtrVoidType OptionalStack,
              _Optional _Input BooleanType Detach,
              _Optional _Input BooleanType Join);

/// @brief Destroys the thread object.
/// @param ThreadPtr 
/// @return 
BooleanType HcDestroyThread(_Input PtrThread ThreadPtr);

/// @brief Stops the thread.
/// @param ThreadPtr 
/// @return 
BooleanType HcStopThread(_Input PtrThread ThreadPtr);

/// @brief Resumes it.
/// @param ThreadPtr 
/// @return 
BooleanType HcResumeThread(_Input PtrThread ThreadPtr);

QWordType HcProcessIdThread(_Input PtrThread ThreadPtr);

/// @brief Main application thread.
CA_EXTERN_C PtrThread kMainThread;

#endif  // __THREAD_API__
