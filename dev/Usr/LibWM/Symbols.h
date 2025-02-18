/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <LibCF/Core.h>

IMPORT_C SInt32 __wm_alert(const char* title, const char* msg, const char* opt_link);
IMPORT_C SInt32 __wm_prompt(const char* title, const char* msg, const char** input, const SizeT input_len);
IMPORT_C SInt32 __wm_exec(const char* path);