/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//
// Created by Amlal on 3/18/24
//

#ifndef __THREAD_API__
#define __THREAD_API__

#include <System.Core/Defs.hxx>

/// @brief Thread Information Block variant for scheduling.
struct ThreadInformationBlock final {
  const CHAR Name[255];        // Module Name
  const UINT_PTR StartAddress;  // Start Address
  const UINT_PTR StartHeap;     // Allocation Heap
  const UINT_PTR StartStack;    // Stack Pointer.
  const DWORD Arch;            // Architecture and/or platform.
  const WORD TID;          // Execution Thread ID.
};

ThreadInformationBlock* HcCreateThread(_Input PVOID Start, 
              _Optional _InOut PVOID HeapOpt, _Optional _InOut PVOID StackOpt);

BOOL HcDestroyThread(_Input ThreadInformationBlock* TIB);

BOOL HcStopThread(_Input ThreadInformationBlock* TIB);

BOOL HcResumeThread(_Input ThreadInformationBlock* TIB);

#endif  // __THREAD_API__
