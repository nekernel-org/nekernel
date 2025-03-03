/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <LibCF/Core.h>

#define kWMPacketLen 512

struct __wm_msg_public;

IMPORT_C SInt32 __wm_alert(const char* title, const char* msg, const char* opt_link);
IMPORT_C SInt32 __wm_prompt(const char* title, const char* msg, const char** input, const SizeT input_len);

IMPORT_C SInt32 __wm_exec(const char* path);

IMPORT_C SInt32 __wm_dispatch_msg(const struct __wm_msg_public* msg, const SizeT len);

/// @brief Public Message Header for the Window Manager.
struct __wm_msg_public
{
	SInt32	   magic;
	SInt32	   version;
	const Char packet[kWMPacketLen];
};