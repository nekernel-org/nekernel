/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <tooling/hefs.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

/// @internal
namespace mkfs::detail {
/// @brief Helper function to get the option value from command line arguments.
template <typename CharType>
static std::basic_string<CharType> get_option(const std::basic_string<CharType>& args,
                                              const std::basic_string<CharType>& option) {
  size_t pos = args.find(option + CharType('='));

  if (pos != std::string::npos) {
    size_t start = pos + option.length() + 1;
    size_t end   = args.find(' ', start);
    return args.substr(start, end - start);
  }

  return std::basic_string<CharType>{};
}
}  // namespace mkfs::detail

static size_t         kDiskSize   = 1024 * 1024 * 1024 * 4UL;
static uint16_t       kVersion    = kHeFSVersion;
static std::u16string kLabel      = kHeFSDefaultVoluneName;
static size_t         kSectorSize = 512;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "mkfs.hefs: Error: Missing required arguments." << std::endl;
    std::cerr << "mkfs.hefs: Usage: mkfs.hefs -L <label> -s <sector_size> -p <part_start> -e "
                 "<part_end> -S <disk_size> -o <output_device>"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string    args;
  std::u16string args_wide;

  for (int i = 1; i < argc; ++i) {
    args += argv[i];
    args += " ";

    std::string str = argv[i];

    for (auto& ch : str) {
      args_wide.push_back(ch);
    }

    args_wide += u" ";
  }

  auto output_device = mkfs::detail::get_option<char>(args, "-o");

  kSectorSize = std::strtol(mkfs::detail::get_option<char>(args, "-s").data(), nullptr, 10);
  kLabel      = mkfs::detail::get_option<char16_t>(args_wide, u"-L");

  if (kLabel.empty()) kLabel = kHeFSDefaultVoluneName;

  kDiskSize = std::strtol(mkfs::detail::get_option<char>(args, "-S").data(), nullptr, 10) * 1024 *
              1024 * 1024;

  if (kDiskSize == 0) {
    std::cerr << "mkfs.hefs: Error: Invalid size specified." << std::endl;
    return EXIT_FAILURE;
  }

  // Open the output_device
  std::ofstream filesystem(output_device, std::ios::binary);

  if (!filesystem.good()) {
    std::cerr << "mkfs.hefs: Error: Unable to open output_device " << output_device << std::endl;
    return EXIT_FAILURE;
  }

  // create a boot node, and then allocate a index node directory tree.
  mkfs::hefs::BootNode bootNode{{}, {}, 0, 0, 0, 0, 0, 0, 0, 0};

  auto start_ind = std::strtol(mkfs::detail::get_option<char>(args, "-p").data(), nullptr, 10);
  start_ind += sizeof(mkfs::hefs::BootNode);

  auto end_ind = std::strtol(mkfs::detail::get_option<char>(args, "-e").data(), nullptr, 10);

  bootNode.version    = kVersion;
  bootNode.diskKind   = mkfs::hefs::kHeFSHardDrive;
  bootNode.encoding   = mkfs::hefs::kHeFSEncodingUTF16;
  bootNode.diskSize   = kDiskSize;
  bootNode.sectorSize = kSectorSize;
  bootNode.startIND   = start_ind;
  bootNode.endIND     = end_ind;
  bootNode.diskStatus = mkfs::hefs::kHeFSStatusUnlocked;

  std::memcpy(bootNode.magic, kHeFSMagic, kHeFSMagicLen);
  std::memcpy(bootNode.volumeName, kLabel.data(), kLabel.size() * sizeof(char16_t));

  filesystem.seekp(std::strtol(mkfs::detail::get_option<char>(args, "-p").data(), nullptr, 10));
  filesystem.write(reinterpret_cast<const char*>(&bootNode), sizeof(mkfs::hefs::BootNode));

  if (!filesystem.good()) {
    std::cerr << "mkfs.hefs: Error: Unable to write to output_device " << output_device
              << std::endl;
    return 1;
  }

  filesystem.seekp(bootNode.startIND);

  auto cnt = end_ind / sizeof(mkfs::hefs::IndexNodeDirectory);

  auto start = bootNode.startIND;
  auto prev = start;

  // Pre-allocate index node directory tree
  for (size_t i = 0; i < cnt; ++i) {
    mkfs::hefs::IndexNodeDirectory indexNode{};

    std::memcpy(indexNode.name, u"/",
                std::u16string(u"/").size() * sizeof(char16_t));

    indexNode.flags   = mkfs::hefs::kHeFSEncodingUTF16;
    indexNode.kind    = mkfs::hefs::kHeFSFileKindDirectory;
    indexNode.deleted = mkfs::hefs::kHeFSTimeMax;

    indexNode.entryCount = 1;

    indexNode.checksum          = 0;
    indexNode.indexNodeChecksum = 0;

    indexNode.uid   = 0;
    indexNode.gid   = 0;
    indexNode.mode  = 0;
    indexNode.color = mkfs::hefs::kHeFSBlack;

    indexNode.parent = bootNode.startIND;
    indexNode.child  = bootNode.endIND;

    indexNode.next = start + sizeof(mkfs::hefs::IndexNodeDirectory);
    indexNode.prev = prev;

    prev = start;
    start += sizeof(mkfs::hefs::IndexNodeDirectory);

    filesystem.write(reinterpret_cast<const char*>(&indexNode),
                     sizeof(mkfs::hefs::IndexNodeDirectory));

    if (!filesystem.good()) {
      std::cerr << "mkfs.hefs: Error: Unable to write index node to output_device " << output_device
                << std::endl;
      return EXIT_FAILURE;
    }
  }

  filesystem.flush();
  filesystem.close();

  std::cout << "mkfs.hefs: HeFS filesystem created in output_device " << output_device << std::endl;

  return EXIT_SUCCESS;
}