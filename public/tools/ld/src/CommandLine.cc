/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 Amlal EL Mahrouss
 */

#include <LibSCI/SCI.h>

/// @brief This program loads a program for NeOS.

SInt32 main(SInt32 argc, Char* argv[])
{
	if (argc < 3)
		return ~0;

	UIntPtr exit_code = RtlSpawnProcess(argv[1], argc - 2, argv + 2,
										nullptr, 0);

	return exit_code;
}
