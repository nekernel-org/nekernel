/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 ZKA Web Services Co
 */

#include <filesystem>
#include <framework.h>

/// @brief This program converts a PE32+ driver, into a custom format, the ZXD.
/// @note ZXD is a format for ZKA signed drivers.
int main(int argc, char* argv[])
{
	for (size_t i = 1ul; i < argc; ++i)
	{
		if (strcmp(argv[i], "/?") == 0)
		{
			std::cout << "make_framework: Framework Creation Tool.\n";
			std::cout << "make_framework: © ZKA Web Services Co, all rights reserved.\n";

			return 0;
		}
	}

	auto path = std::string(argv[1]);

	if (!path.ends_with(kFKExtension))
		return 1;

	std::filesystem::path path_arg = path;

	if (std::filesystem::create_directory(path_arg))
	{
		std::filesystem::create_directory(path_arg / kFKRootDirectory);
		std::filesystem::create_directory(path_arg / kFKManifestDirectory);
		std::filesystem::create_directory(path_arg / kFKDLLDirectory);

		return 0;
	}

	return 1;
}
