/*
 * Created on Thu Aug 22 09:29:13 CEST 2024
 *
 * Copyright (c) 2024 ZKA Technologies
 */

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <filesystem>

#define kDriverSignedExt   ".zxd"
#define kDriverExt		   ".sys"
#define kSignedDriverMagic " ZXD"

namespace ZXD
{
	struct ZXD_HEADER final
	{
		char d_binary_padding[512];
		// doesn't change.
		char d_binary_magic[5];
		int	 d_binary_version;
		// can change.
		char		  d_binary_name[4096];
		std::uint64_t d_binary_checksum;
		std::uint64_t d_binary_size;
	};

	/***********************************************************************************/
	/* This handles the detection of a MZ header. */
	/***********************************************************************************/

	bool drvsign_check_for_mz(std::string mz_blob) noexcept
	{
		return mz_blob[0] == 'M' &&
			   mz_blob[1] == 'Z';
	}
} // namespace ZXD

/// @brief This program converts a PE32+ driver, into a custom format, the ZXD.
/// @note ZXD is a format for ZKA signed drivers.
int main(int argc, char* argv[])
{
	for (size_t i = 1ul; i < argc; ++i)
	{
		if (strcmp(argv[i], "/?") == 0)
		{
			std::cout << "drvsign: ZKA ZXD Driver Tool.\n";
			std::cout << "drvsign: © ZKA Technologies, all rights reserved.\n";

			return 0;
		}
	}

	if (!std::filesystem::exists(argv[1]) ||
		!std::string(argv[1]).ends_with(kDriverExt))
		return -1;

	ZXD::ZXD_HEADER sig{0};

	sig.d_binary_version = 1;

	memcpy(sig.d_binary_magic, kSignedDriverMagic, strlen(kSignedDriverMagic));
	memcpy(sig.d_binary_name, argv[1], strlen(argv[1]));

	sig.d_binary_size = std::filesystem::file_size(argv[1]);

	memset(sig.d_binary_padding, 0x00, 512);

	sig.d_binary_checksum = 0;

	std::string signed_path = argv[1];
	signed_path.erase(signed_path.find(kDriverExt), strlen(kDriverExt));
	signed_path += kDriverSignedExt;

	std::ofstream of_drv(signed_path, std::ios::binary);
	std::ifstream if_drv(argv[1], std::ios::binary);

	std::stringstream ss;
	ss << if_drv.rdbuf();

	if (!ZXD::drvsign_check_for_mz(ss.str()))
	{
		std::filesystem::remove(signed_path);
		std::cout << "drvsign: Couldn't sign current driver, Input driver isn't a valid executable.\n";

		return 1;
	}

	for (auto ch : ss.str())
	{
		sig.d_binary_checksum |= ch;
	}

	sig.d_binary_checksum ^= sig.d_binary_size;

	of_drv.write((char*)&sig, sizeof(ZXD::ZXD_HEADER));
	of_drv.write(ss.str().c_str(), ss.str().size());

	std::cout << "drvsign: Signing is done, quiting, here is the key: " << sig.d_binary_checksum << ".\n";

	return 0;
}
