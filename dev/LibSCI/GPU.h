/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

File: GPU.h
Purpose: GPU/FB System Calls.

------------------------------------------- */

#ifndef SCI_GPU_H
#define SCI_GPU_H

#include <LibSCI/SCI.h>

/// ------------------------------------------------------------------------------------------ //
/// @brief GPU API.
/// ------------------------------------------------------------------------------------------ //

struct GPUCmd;

typedef VoidPtr GPUObject;

/// ------------------------------------------------------------------------------------------ //
/// @brief Command structure type.
/// ------------------------------------------------------------------------------------------ //

struct GPUCmd
{
	VoidPtr Data{nullptr};
	SizeT	DataSz{0};
	SizeT	BufferLayer{0};
	BOOL	IsGPGPUData{NO};

	BOOL isGPGPUData()
	{
		return this->isValid() && this->IsGPGPUData;
	}

	BOOL isBackBuffer()
	{
		return this->BufferLayer > 0;
	}

	BOOL isValid()
	{
		return this->Data && (this->DataSz > 0) && (MmGetHeapFlags(this->Data) != -1);
	}
};

IMPORT_C BOOL GPUListDevices(const Char** list, SizeT cnt);

IMPORT_C GPUObject GPUGetFromDeviceName(_Input const Char* device_name);

IMPORT_C GPUObject GPUQueryInfo(_Input const GPUObject* device_name, VoidPtr* out, SizeT out_sz);

IMPORT_C SInt32 GPUDisposeDevice(GPUObject gpu_handle, Bool cancel_all, Bool flush_all);

IMPORT_C SInt32 GPUSendCmdBufferListWithCnt(GPUCmd** cmd_list, SizeT cmd_list_cnt);

// ------------------------------------------------------------------------------------------ //
// @brief FB API.
// ------------------------------------------------------------------------------------------ //

IMPORT_C SInt32 FBAcquireBuffer(GPUObject* out, SInt32 width, SInt32 height);

IMPORT_C SInt32 FBDisposeBuffer(GPUObject* in);

IMPORT_C SInt32 FBGetColorProfile(GPUObject in);

IMPORT_C SInt32 FBQueryInfo(GPUObject handle, VoidPtr* info_ptr, SizeT* info_sz);

#endif // ifndef SCI_GPU_H
