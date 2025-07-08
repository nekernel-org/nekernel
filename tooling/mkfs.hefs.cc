/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <tooling/libmkfs/hefs.h>
#include <tooling/libmkfs/mkfs.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>

static size_t        kDiskSize = mkfs::detail::gib_cast(4UL);
static uint16_t      kVersion  = kHeFSVersion;
static std::u8string kLabel;
static size_t        kSectorSize = 512;

int main(int argc, char** argv) {
  if (argc < 2) {
    mkfs::console_out()
        << "hefs: usage: mkfs.hefs -L=<label> -s=<sector_size> -b=<ind_start> -e=<ind_end> "
           "-bs=<block_start> -be=<block_end> -is=<in_start> -ie=<in_end> "
           "-S=<disk_size_GB> -o=<output_device>\n";

    return EXIT_FAILURE;
  }

  std::string args = mkfs::detail::build_args(argc, argv);

  auto output_path = mkfs::get_option<char>(args, "-o");
  if (output_path.empty()) {
    mkfs::console_out() << "hefs: error: Missing -o <output_device> argument.\n";
    return EXIT_FAILURE;
  }

  auto opt_s    = mkfs::get_option<char>(args, "-s");
  long parsed_s = 0;
  if (!mkfs::detail::parse_signed(opt_s, parsed_s, 10) || parsed_s == 0) {
    mkfs::console_out() << "hefs: error: Invalid sector size \"" << opt_s
                        << "\". Must be a positive integer.\n";
    return EXIT_FAILURE;
  }

  if ((parsed_s & (parsed_s - 1)) != 0) {
    mkfs::console_out() << "hefs: error: Sector size \"" << parsed_s
                        << "\" is not a power of two.\n";
    return EXIT_FAILURE;
  }
  kSectorSize = static_cast<size_t>(parsed_s);

  auto opt_L = mkfs::get_option<char>(args, "-L");
  if (!opt_L.empty()) {
    kLabel.clear();
    for (char c : opt_L) kLabel.push_back(static_cast<char8_t>(c));
  } else {
    kLabel.clear();
    for (size_t i = 0; i < kHeFSPartNameLen && kHeFSDefaultVolumeName[i] != u'\0'; ++i) {
      kLabel.push_back(static_cast<char8_t>(kHeFSDefaultVolumeName[i]));
    }
  }

  auto               opt_S = mkfs::get_option<char>(args, "-S");
  unsigned long long gb    = 0;
  if (!mkfs::detail::parse_decimal(opt_S, gb) || gb == 0ULL) {
    mkfs::console_out() << "hefs: error: Invalid disk size \"" << opt_S
                        << "\". Must be a positive integer.\n";
    return EXIT_FAILURE;
  }
  unsigned long long max_gb = std::numeric_limits<uint64_t>::max() / (1024ULL * 1024ULL * 1024ULL);
  if (gb > max_gb) {
    mkfs::console_out() << "hefs: error: Disk size \"" << gb << "GB\" is too large.\n";
    return EXIT_FAILURE;
  }
  kDiskSize = static_cast<size_t>(gb * 1024ULL * 1024ULL * 1024ULL);

  auto opt_b  = mkfs::get_option<char>(args, "-b");
  auto opt_e  = mkfs::get_option<char>(args, "-e");
  auto opt_bs = mkfs::get_option<char>(args, "-bs");
  auto opt_be = mkfs::get_option<char>(args, "-be");
  auto opt_is = mkfs::get_option<char>(args, "-is");
  auto opt_ie = mkfs::get_option<char>(args, "-ie");

  long start_ind = 0, end_ind = 0;
  long start_block = 0, end_block = 0;
  long start_in = 0, end_in = 0;

  if (!mkfs::detail::parse_signed(opt_b, start_ind, 16)) {
    mkfs::console_out() << "hefs: error: Invalid -b <hex> argument.\n";
    return EXIT_FAILURE;
  }
  if (!mkfs::detail::parse_signed(opt_e, end_ind, 16) || end_ind <= start_ind) {
    mkfs::console_out() << "hefs: error: Invalid or out-of-range -e <hex> argument.\n";
    return EXIT_FAILURE;
  }
  if (!mkfs::detail::parse_signed(opt_bs, start_block, 16)) {
    mkfs::console_out() << "hefs: error: Invalid -bs <hex> argument.\n";
    return EXIT_FAILURE;
  }
  if (!mkfs::detail::parse_signed(opt_be, end_block, 16) || end_block <= start_block) {
    mkfs::console_out() << "hefs: error: Invalid or out-of-range -be <hex> argument.\n";
    return EXIT_FAILURE;
  }
  if (!mkfs::detail::parse_signed(opt_is, start_in, 16)) {
    mkfs::console_out() << "hefs: error: Invalid -is <hex> argument.\n";
    return EXIT_FAILURE;
  }
  if (!mkfs::detail::parse_signed(opt_ie, end_in, 16) || end_in <= start_in) {
    mkfs::console_out() << "hefs: error: Invalid or out-of-range -ie <hex> argument.\n";
    return EXIT_FAILURE;
  }

  if (static_cast<size_t>(end_block) * kSectorSize > kDiskSize ||
      static_cast<size_t>(end_ind) > kDiskSize || static_cast<size_t>(end_in) > kDiskSize) {
    mkfs::console_out() << "hefs: error: One or more ranges exceed disk size.\n";
    return EXIT_FAILURE;
  }

  std::ofstream output_device(output_path, std::ios::binary);

  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Unable to open output device: " << output_path << "\n";
    return EXIT_FAILURE;
  }

  mkfs::hefs::BootNode boot_node;
  std::memset(&boot_node, 0, sizeof(boot_node));

  boot_node.version    = kVersion;
  boot_node.diskKind   = mkfs::hefs::kHeFSHardDrive;
  boot_node.encoding   = mkfs::hefs::kHeFSEncodingFlagsUTF8;
  boot_node.diskSize   = kDiskSize;
  boot_node.sectorSize = kSectorSize;
  boot_node.startIND   = static_cast<size_t>(start_ind) + sizeof(mkfs::hefs::BootNode);
  boot_node.endIND     = static_cast<size_t>(end_ind);
  boot_node.startIN    = static_cast<size_t>(start_in);
  boot_node.endIN      = static_cast<size_t>(end_in);
  boot_node.startBlock = static_cast<size_t>(start_block);
  boot_node.endBlock   = static_cast<size_t>(end_block);
  boot_node.indCount   = 0UL;
  boot_node.diskStatus = mkfs::hefs::kHeFSStatusUnlocked;

  static_assert(sizeof(boot_node.magic) >= kHeFSMagicLen,
                "BootNode::magic too small to hold kHeFSMagicLen");
  std::memset(boot_node.magic, 0, sizeof(boot_node.magic));
  size_t magic_copy =
      (sizeof(boot_node.magic) < kHeFSMagicLen - 1) ? sizeof(boot_node.magic) : (kHeFSMagicLen - 1);
  std::memcpy(boot_node.magic, kHeFSMagic, magic_copy);
  boot_node.magic[magic_copy] = 0;

  constexpr size_t vol_slots = kHeFSPartNameLen;
  
  std::memset(boot_node.volumeName, 0, sizeof(boot_node.volumeName));
  
  size_t label_units = std::min(kLabel.size(), vol_slots - 1);

  for (size_t i = 0; i < label_units; ++i) {
    boot_node.volumeName[i] = static_cast<char8_t>(kLabel[i]);
  }
  
  boot_node.volumeName[label_units] = 0U;

  output_device.seekp(static_cast<std::streamoff>(start_ind));
  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Failed seek to index start.\n";
    return EXIT_FAILURE;
  }

  output_device.write(reinterpret_cast<const char*>(&boot_node), sizeof(boot_node));
  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Unable to write BootNode to output device: " << output_path
                        << "\n";
    return EXIT_FAILURE;
  }

  output_device.seekp(static_cast<std::streamoff>(boot_node.startIND));
  if (!output_device.good()) {
    mkfs::console_out() << "hefs: error: Failed seek to startIND.\n";
    return EXIT_FAILURE;
  }

  output_device.flush();
  output_device.close();

  mkfs::console_out() << "hefs: info: Wrote filesystem to output device: " << output_path << "\n";
  return EXIT_SUCCESS;
}
