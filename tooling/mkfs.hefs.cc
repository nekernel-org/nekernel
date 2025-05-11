/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <tooling/hefs.h>
#include <tooling/mkfs.h>
#include <cstdlib>
#include <fstream>

namespace detail {
/// @interal
/// @brief GB equation formula.
static constexpr size_t gib_cast(uint32_t gb) {
  return ((1024 ^ 3) * gb);
}
}  // namespace detail

static size_t        kDiskSize   = detail::gib_cast(4UL);
static uint16_t      kVersion    = kHeFSVersion;
static std::u8string kLabel      = kHeFSDefaultVolumeName;
static size_t        kSectorSize = 512;

/// @brief Entrypoint of tool.
int main(int argc, char** argv) {
  if (argc < 2) {
    mkfs::console_out()
        << "hefs: usage: mkfs.hefs -L <label> -s <sector_size> -b <ind_start> -e "
        << "<ind_end> -bs <block_start> -be <block_end> -is <in_start> -ie <in_end> "
           "-S <disk_size> -o <output_device>"
        << "\n";
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

  auto output_path = mkfs::get_option<char>(args, "-o");

  kSectorSize = std::strtol(mkfs::get_option<char>(args, "-s").data(), nullptr, 10);
  kLabel      = mkfs::get_option<char8_t>(args_wide, u8"-L");

  if (!kSectorSize) {
    mkfs::console_out() << "hefs: error: Sector size size is zero.\n";
    return EXIT_FAILURE;
  }

  if (kLabel.empty()) kLabel = kHeFSDefaultVolumeName;

  kDiskSize =
      std::strtol(mkfs::get_option<char>(args, "-S").data(), nullptr, 10) * 1024 * 1024 * 1024;

  if (!kDiskSize) {
    mkfs::console_out() << "hefs: error: Disk size is zero.\n";
    return EXIT_FAILURE;
  }

  // Open the output_device
  std::ofstream output_device(output_path, std::ios::binary);

  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Unable to open output_device: " << output_path << "\n";
    return EXIT_FAILURE;
  }

  // create a boot node, and then allocate a index node directory tree.
  mkfs::hefs::BootNode boot_node{{}, {}, 0, 0, 0, 0, 0, 0, 0, 0};

  auto start_ind = std::strtol(mkfs::get_option<char>(args, "-b").data(), nullptr, 16);

  start_ind += sizeof(mkfs::hefs::BootNode);

  auto end_ind = std::strtol(mkfs::get_option<char>(args, "-e").data(), nullptr, 16);

  auto start_block = std::strtol(mkfs::get_option<char>(args, "-bs").data(), nullptr, 16);
  auto end_block   = std::strtol(mkfs::get_option<char>(args, "-be").data(), nullptr, 16);

  auto start_in = std::strtol(mkfs::get_option<char>(args, "-is").data(), nullptr, 16);
  auto end_in   = std::strtol(mkfs::get_option<char>(args, "-ie").data(), nullptr, 16);

  boot_node.version    = kVersion;
  boot_node.diskKind   = mkfs::hefs::kHeFSHardDrive;
  boot_node.encoding   = mkfs::hefs::kHeFSEncodingFlagsUTF8;
  boot_node.diskSize   = kDiskSize;
  boot_node.sectorSize = kSectorSize;
  boot_node.startIND   = start_ind;
  boot_node.endIND     = end_ind;
  boot_node.startIN    = start_in;
  boot_node.endIN      = end_in;
  boot_node.startBlock = start_block;
  boot_node.endBlock   = end_block;
  boot_node.indCount   = 0UL;
  boot_node.diskStatus = mkfs::hefs::kHeFSStatusUnlocked;

  std::memcpy(boot_node.magic, kHeFSMagic, kHeFSMagicLen - 1);
  std::memcpy(boot_node.volumeName, kLabel.data(), kLabel.size() * sizeof(char16_t));

  output_device.seekp(std::strtol(mkfs::get_option<char>(args, "-b").data(), nullptr, 16));
  output_device.write(reinterpret_cast<const char*>(&boot_node), sizeof(mkfs::hefs::BootNode));

  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Unable to write filesystem to output_device: "
                        << output_path << "\n";
    return EXIT_FAILURE;
  }

  output_device.seekp(boot_node.startIND);

  output_device.flush();
  output_device.close();

  mkfs::console_out() << "hefs: info: Wrote filesystem to output_device: " << output_path << "\n";

  return EXIT_SUCCESS;
}