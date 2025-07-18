/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <tooling/rang.h>
#include <sstream>
#include <iostream>
#include <string>

#define kMkFsSectorSz (512U)
#define kMkFsMaxBadSectors (128U)

/// @internal
namespace mkfs {

namespace detail {
  /// @internal
  /// @brief GB‐to‐byte conversion (use multiplication, not XOR).
  inline constexpr size_t gib_cast(uint32_t gb) {
    return static_cast<size_t>(gb) * 1024ULL * 1024ULL * 1024ULL;
  }

  inline bool parse_decimal(const std::string& opt, unsigned long long& out) {
    if (opt.empty()) return false;
    char*              endptr = nullptr;
    unsigned long long val    = std::strtoull(opt.c_str(), &endptr, 10);
    if (endptr == opt.c_str() || *endptr != '\0') return false;
    out = val;
    return true;
  }

  inline bool parse_signed(const std::string& opt, long& out, int base = 10) {
    if (opt.empty()) return false;
    char* endptr = nullptr;
    long  val    = std::strtol(opt.c_str(), &endptr, base);
    if (endptr == opt.c_str() || *endptr != '\0' || val < 0) return false;
    out = val;
    return true;
  }

  inline std::string build_args(int argc, char** argv) {
    std::string combined;
    for (int i = 1; i < argc; ++i) {
      combined += argv[i];
      combined += ' ';
    }
    return combined;
  }
}  // namespace detail

/// @brief Helper function to get the option value from command line arguments.
template <typename CharType>
inline std::basic_string<CharType> get_option(const std::basic_string<CharType>& args,
                                              const std::basic_string<CharType>& option) {
  size_t pos = args.find(option + CharType('='));

  if (pos != std::string::npos) {
    size_t start = pos + option.length() + 1;
    size_t end   = args.find(' ', start);
    return args.substr(start, end - start);
  }

  return std::basic_string<CharType>{};
}

inline auto console_out() -> std::ostream& {
  std::ostream& conout = std::cout;
  conout << rang::fg::red << "mkfs: " << rang::style::reset;

  return conout;
}
}  // namespace mkfs