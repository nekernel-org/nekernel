/*
 * Created on Thu Aug 22 09:29:13 CEST 2024
 *
 * Copyright (c) 2024 ZKA Web Services Co
 */

#include <zxd.hxx>

/// @brief This program converts a PE32+ driver, into a custom format, the ZXD.
/// @note ZXD is a format for ZKA signed drivers.
int main(int argc, char* argv[])
{
	for (size_t i = 1ul; i < argc; ++i)
	{
		if (strcmp(argv[i], "/?") == 0)
		{
			std::cout << "make_zxd: ZXD Tool.\n";
			std::cout << "make_zxd: © ZKA Web Services Co, all rights reserved.\n";

			return 0;
		}
	}

	if (!std::filesystem::exists(argv[1]) ||
		!std::string(argv[1]).ends_with(kDriverExt))
		return -1;

	ZXD::ZXD_HEADER zxd_hdr{0};

	zxd_hdr.d_binary_version = 1;

	memcpy(zxd_hdr.d_binary_magic, kSignedDriverMagic, strlen(kSignedDriverMagic));
	memcpy(zxd_hdr.d_binary_name, argv[1], strlen(argv[1]));

	zxd_hdr.d_binary_size = std::filesystem::file_size(argv[1]);

	memset(zxd_hdr.d_binary_padding, 0x00, 512);

	zxd_hdr.d_binary_checksum = 0;

	std::string signed_path = argv[1];
	signed_path.erase(signed_path.find(kDriverExt), strlen(kDriverExt));
	signed_path += kDriverSignedExt;

	std::ofstream of_drv(signed_path, std::ios::binary);
	std::ifstream if_drv(argv[1], std::ios::binary);

	std::stringstream ss;
	ss << if_drv.rdbuf();

	if (!ZXD::zxd_check_for_mz(ss.str()))
	{
		std::filesystem::remove(signed_path);
		std::cout << "zxdmake: Couldn't sign current driver, Input driver isn't a valid executable.\n";

		return 1;
	}

	for (auto ch : ss.str())
	{
		zxd_hdr.d_binary_checksum |= ch;
	}

	zxd_hdr.d_binary_checksum ^= zxd_hdr.d_binary_size;

	of_drv.write((char*)&zxd_hdr, sizeof(ZXD::ZXD_HEADER));
	of_drv.write(ss.str().c_str(), ss.str().size());

	std::cout << "zxdmake: Signing is done, quiting, Checksum: " << zxd_hdr.d_binary_checksum << ".\n";

	return 0;
}
