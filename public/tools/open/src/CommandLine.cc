/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024-2025 Amlal El Mahrouss
 */

#include <user/SystemCalls.h>

/// @brief This program opens an application from **OPEN_APP_BASE_PATH**
/// @file CommandLine.cc

#define OPEN_APP_APP_FLAG  "-a"
#define OPEN_APP_HELP_FLAG "-h"
#define OPEN_APP_BASE_PATH "/app/"

SInt32 main(SInt32 argc, Char* argv[])
{
	if (argc == 1)
		return EXIT_FAILURE;

	for (SizeT i = 1UL; i < argc; ++i)
	{
		if (MmStrCmp(argv[i], OPEN_APP_HELP_FLAG) == 0)
		{
			PrintOut(nullptr, "open: open .app(s) directories.\n");
			PrintOut(nullptr, "open: © 2024-2025 Amlal El Mahrouss, All rights reserved.\n");

			PrintOut(nullptr, "open: %s: Application is being taken as the input (opens a PEF/PE32+/ELF program depending on the CPU architecture).\n", OPEN_APP_APP_FLAG);

			return EXIT_SUCCESS;
		}
		else if (MmStrCmp(argv[i], OPEN_APP_APP_FLAG) == 0)
		{
			if ((i + 1) == argc)
				return EXIT_FAILURE;
			else if ((i + 2) == argc)
				return EXIT_FAILURE;

			Char base_path[FILE_MAX_LEN] = OPEN_APP_BASE_PATH;
			MmCopyMemory(base_path + MmStrLen(OPEN_APP_BASE_PATH), argv[i + 1], MmStrLen(argv[i + 1]));

			UIntPtr ret = RtlSpawnProcess(StrFmt("{}/dist/{]}", base_path, argv[i + 2]), 0, nullptr, nullptr, 0);

			return ret;
		}
	}

	return EXIT_FAILURE;
}
