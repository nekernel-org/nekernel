/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 t& Corporation
 */

#include <iostream>
#include <vector>

/// @brief This program makes a framework directory for ZKA OS.

int main(int argc, char* argv[])
{
	if (argc == 1)
		return EXIT_FAILURE;

	for (size_t i = 1UL; i < argc; ++i)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			std::cout << "open: Open Application Tool.\n";
			std::cout << "open: © t& Corporation, All rights reserved.\n";

			std::cout << "open: -a: Application is taken as input.\n";
			std::cout << "open: -u: URL is taken as input.\n";

			return EXIT_SUCCESS;
		}
		else if (strcmp(argv[i], "-a") == 0)
		{
			const auto kBasePath = "/apps/packages/";

			return EXIT_SUCCESS;
		}
		else if (strcmp(argv[i], "-u") == 0)
		{
			const auto kBasePath = "/support/defaults/browser";

			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}
