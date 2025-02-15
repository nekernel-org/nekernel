/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

File: GPU.h
Purpose: GFX System Calls.

------------------------------------------- */

#ifndef SCI_GPU_H
#define SCI_GPU_H

#include <LibSCI/SCI.h>

/// ------------------------------------------------------------------------------------------ //
/// @brief GPU API.
/// ------------------------------------------------------------------------------------------ //

struct GPUCmdBuffer;

typedef VoidPtr GPUObject;

/// ------------------------------------------------------------------------------------------ //
/// @brief Command buffer structure type.
/// ------------------------------------------------------------------------------------------ //
struct GPUCmdBuffer final
{
	VoidPtr Data{nullptr};
	SizeT	DataSz{0};
	SizeT	BufferLayer{0};
	Bool	IsGPGPUData{false};
	Bool	BufferFirst{false};

	Bool isGPGPUData()
	{
		return this->isValid() && !this->BufferFirst && this->IsGPGPUData;
	}

	Bool isBackBuffer()
	{
		return !this->BufferFirst;
	}

	Bool isValid()
	{
		return this->Data && (this->DataSz > 0) && (MmGetHeapFlags(this->Data) != -1);
	}
};

IMPORT_C GPUObject GPUNewFromDeviceName(_Input const Char* device_name);

IMPORT_C SInt32 GPUDisposeDevice(GPUObject gpu_handle, Bool cancel_all, Bool flush_all);

IMPORT_C SInt32 GPUSendCmdBufferListWithCnt(GPUCmdBuffer** cmd_list, SizeT cmd_list_cnt);

#endif // ifndef SCI_GPU_H
