/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 ELMH GROUP
 */

#include <cstdlib>
#include <filesystem>
#include <Framework.h>
#include <vector>

/// @brief This program makes a framework directory for ZKA OS.

int main(int argc, char* argv[])
{
	std::vector<std::string> files;

	auto ext = kFKExtension;

	for (size_t i = 2UL; i < argc; ++i)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			std::cout << "make_framework: Framework/Application Creation Tool.\n";
			std::cout << "make_framework: © ELMH GROUP, all rights reserved.\n";

			return EXIT_SUCCESS;
		}

		if (strcmp(argv[i], "-a") == 0)
		{
			ext = kAppExtension;
			continue;
		}

		files.push_back(argv[i]);
	}

	auto path = std::string(argv[1]);

	if (!path.ends_with(ext))
		return EXIT_FAILURE;

	std::filesystem::path path_arg = path;

	if (std::filesystem::create_directory(path_arg))
	{
		std::filesystem::create_directory(path_arg / kRootDirectory);
		std::filesystem::create_directory(path_arg / kExecDirectory);

		for (auto& file : files)
		{
			std::string file_cpy = file;

			while (file_cpy.find("/") != std::string::npos)
			{
				file_cpy.erase(0, file_cpy.find("/"));
				file_cpy.erase(file_cpy.find("/"), 1);
			}

			std::filesystem::copy(path, path_arg / kExecDirectory / file_cpy);
		}

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
