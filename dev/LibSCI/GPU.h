/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

File: GPU.h
Purpose: GPU/FB System Calls.

------------------------------------------- */

#ifndef SCI_GPU_H
#define SCI_GPU_H

#include <LibSCI/SCI.h>

/// ------------------------------------------------------------------------------------------ //
/// @brief Graphics Processor API.
/// ------------------------------------------------------------------------------------------ //

struct GPUCmdEntry;
struct GPUCmdEntryTraits;

typedef VoidPtr GPUObject;

/// ------------------------------------------------------------------------------------------ //
/// @brief Command structure type.
/// ------------------------------------------------------------------------------------------ //

struct GPUCmdEntry
{
	VoidPtr Data{nullptr};
	SizeT	DataSz{0};
	SizeT	Layer{0};
	BOOL	IsGPGPUData{NO};
};

struct GPUCmdEntryTraits final
{
	GPUCmdEntry* Cmd{nullptr};

	BOOL isGPGPUData()
	{
		return this->isValid() && this->Cmd->IsGPGPUData;
	}

	BOOL isBackBuffer()
	{
		return this->Cmd->Layer > 0;
	}

	BOOL isValid()
	{
		return this->Cmd->Data && (this->Cmd->DataSz > 0) && (MmGetHeapFlags(this->Cmd->Data) != ~0);
	}
};

IMPORT_C BOOL GPUListDevices(const Char** list, SizeT cnt);

IMPORT_C GPUObject GPUFindByDeviceName(_Input const Char* device_name);

IMPORT_C GPUObject GPUQueryInfo(_Input const GPUObject* gpu_handle, VoidPtr* out, SizeT out_sz);

IMPORT_C SInt32 GPUDisposeDevice(GPUObject gpu_handle, Bool cancel_all, Bool flush_all);

IMPORT_C SInt32 GPUSendCmdBufferListWithCnt(GPUObject gpu_handle, GPUCmdEntry** cmd_list, SizeT cmd_list_cnt);

// ------------------------------------------------------------------------------------------ //
// @brief Framebuffer API.
// ------------------------------------------------------------------------------------------ //

IMPORT_C SInt32 FBAcquireBuffer(GPUObject* out, SInt32 width, SInt32 height);

IMPORT_C SInt32 FBDisposeBuffer(GPUObject* in);

IMPORT_C SInt32 FBGetColorProfile(GPUObject in);

IMPORT_C SInt32 FBQueryInfo(GPUObject handle, VoidPtr* info_ptr, SizeT* info_sz);

#endif // ifndef SCI_GPU_H
