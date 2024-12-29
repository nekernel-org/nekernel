/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 Theater Quality Corp
 */

#include <Framework.h>
#include <Steps.h>

/// @brief This program makes a framework directory for ZKA OS.

int main(int argc, char* argv[])
{
	std::vector<std::string> files;

	auto ext = kFKExtension;

	for (size_t i = 2UL; i < argc; ++i)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			std::cout << "make_container: Framework/Application Creation Tool.\n";
			std::cout << "make_container: © Theater Quality Corp, All rights reserved.\n";

			std::cout << "make_container: -app: Application directory.\n";
			std::cout << "make_container: -steps: Steps directory.\n";
			std::cout << "make_container: -fwrk: Framework directory.\n";

			return EXIT_SUCCESS;
		}

		if (strcmp(argv[i], "-app") == 0)
		{
			ext = kAppExtension;
			continue;
		}
		else if (strcmp(argv[i], "-steps") == 0)
		{
			ext = kStepsExtension;
			continue;
		}
		else if (strcmp(argv[i], "-fwrk") == 0)
		{
			ext = kFKExtension;
			continue;
		}

		files.push_back(argv[i]);
	}

	auto path = std::string(argv[1]);

	if (!std::filesystem::exists(path))
		return EXIT_FAILURE;

	std::filesystem::path path_arg = path + ext;

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
