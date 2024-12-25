/* -------------------------------------------

Copyright (C) 2024, Theater Quality Inc, all rights reserved.

File: Macros.h
Purpose: SCIKit Macros header.

------------------------------------------- */

#pragma once

#include <Macros.h>

/// @brief In this file we define filesystem calls.

SYSCALL_DECL(FCloseFile, const SCIObject handle);
SYSCALL_DECL(FOpenFile, const Char* name, const Char* rest);
SYSCALL_DECL(FSeekFile, const SCIObject handle, const SizeT pos);
SYSCALL_DECL(FReadFile, const SCIObject handle, const Char* out_buf, const SizeT out_buf_size);
SYSCALL_DECL(FWriteFile, const SCIObject handle, const Char* out_buf, const SizeT out_buf_size);
