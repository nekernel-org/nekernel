/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <tooling/hefs.h>
#include <tooling/mkfs.h>
#include <cstdlib>
#include <fstream>

static size_t        kDiskSize   = 1024 * 1024 * 1024 * 4UL;
static uint16_t      kVersion    = kHeFSVersion;
static std::u8string kLabel      = kHeFSDefaultVoluneName;
static size_t        kSectorSize = 512;

int main(int argc, char** argv) {
  if (argc < 2) {
    mkfs::console_out() << "hefs: Usage: mkfs.hefs -L <label> -s <sector_size> -p <part_start> -e "
                        << "<part_end> -S <disk_size> -o <output_device>" << std::endl;

    return EXIT_FAILURE;
  }

  std::string   args;
  std::u8string args_wide;

  for (int i = 1; i < argc; ++i) {
    args += argv[i];
    args += " ";

    std::string str = argv[i];

    for (auto& ch : str) {
      args_wide.push_back(ch);
    }

    args_wide += u8" ";
  }

  auto output_device = mkfs::get_option<char>(args, "-o");

  kSectorSize = std::strtol(mkfs::get_option<char>(args, "-s").data(), nullptr, 10);
  kLabel      = mkfs::get_option<char8_t>(args_wide, u8"-L");

  if (kLabel.empty()) kLabel = kHeFSDefaultVoluneName;

  kDiskSize =
      std::strtol(mkfs::get_option<char>(args, "-S").data(), nullptr, 10) * 1024 * 1024 * 1024;

  if (kDiskSize == 0) {
    mkfs::console_out() << "hefs: Error: Unable to deduce future disk size for output_device: "
                        << output_device << std::endl;
    return EXIT_FAILURE;
  }

  // Open the output_device
  std::ofstream filesystem(output_device, std::ios::binary);

  if (!filesystem.good()) {
    mkfs::console_out() << "hefs: Info: Unable to open output_device: " << output_device
                        << std::endl;
    return EXIT_FAILURE;
  }

  // create a boot node, and then allocate a index node directory tree.
  mkfs::hefs::BootNode bootNode{{}, {}, 0, 0, 0, 0, 0, 0, 0, 0};

  auto start_ind = std::strtol(mkfs::get_option<char>(args, "-p").data(), nullptr, 10);

  start_ind += sizeof(mkfs::hefs::BootNode);

  auto end_ind = std::strtol(mkfs::get_option<char>(args, "-e").data(), nullptr, 10);

  bootNode.version    = kVersion;
  bootNode.diskKind   = mkfs::hefs::kHeFSHardDrive;
  bootNode.encoding   = mkfs::hefs::kHeFSEncodingFlagsUTF8;
  bootNode.diskSize   = kDiskSize;
  bootNode.sectorSize = kSectorSize;
  bootNode.startIND   = start_ind;
  bootNode.endIND     = end_ind;
  bootNode.indCount   = 0UL;
  bootNode.diskStatus = mkfs::hefs::kHeFSStatusUnlocked;

  std::memcpy(bootNode.magic, kHeFSMagic, kHeFSMagicLen - 1);
  std::memcpy(bootNode.volumeName, kLabel.data(), kLabel.size() * sizeof(char16_t));

  filesystem.seekp(std::strtol(mkfs::get_option<char>(args, "-p").data(), nullptr, 10));
  filesystem.write(reinterpret_cast<const char*>(&bootNode), sizeof(mkfs::hefs::BootNode));

  if (!filesystem.good()) {
    mkfs::console_out() << "hefs: Error: Unable to write FS to output_device: " << output_device
                        << std::endl;
    return EXIT_FAILURE;
  }

  filesystem.seekp(bootNode.startIND);

  filesystem.flush();
  filesystem.close();

  mkfs::console_out() << "hefs: Info: Wrote FS to output_device: " << output_device << std::endl;

  return EXIT_SUCCESS;
}