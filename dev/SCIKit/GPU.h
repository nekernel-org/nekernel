/* -------------------------------------------

Copyright (C) 2024, TQ B.V, all rights reserved.

File: GPU.h
Purpose: GFX System Calls.

------------------------------------------- */

#ifndef SCIKIT_GPU_H
#define SCIKIT_GPU_H

#include <SCIKit/SCI.h>

// ------------------------------------------------------------------------------------------ //
// GPU API.
// ------------------------------------------------------------------------------------------ //

// ------------------------------------------------------------------------------------------ //
// @brief Command buffer structure type.
// ------------------------------------------------------------------------------------------ //
struct GPU_CMD_BUFFER final
{
	SizeT	X, Y, Z;
	VoidPtr FrameData;
	SizeT	FrameDataSz;
	SizeT	BackBufferLayer;
	Bool	IsGPGPUData;
	Bool	BackBufferFirst;

	Bool isGPGPUData()
	{
		return !this->BackBufferFirst && this->IsGPGPUData;
	}

	Bool isValid()
	{
		return this->FrameData && this->FrameDataSz > 0;
	}
};

typedef VoidPtr GPUObject;

IMPORT_C GPUObject GPUNewFromDeviceName(_Input const Char* device_name);

IMPORT_C SInt32 GPUDisposeDevice(GPUObject gpu_handle, Bool cancel_all, Bool flush_all);

IMPORT_C SInt32 GPUSendCmdBufferListWithCnt(GPU_CMD_BUFFER** cmd_list, SizeT cmd_list_cnt);

#endif // ifndef SCIKIT_GPU_H
