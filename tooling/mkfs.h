/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <iostream>
#include <string>
#include <tooling/rang.h>

/// @internal
namespace mkfs {
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