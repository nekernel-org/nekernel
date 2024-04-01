
/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

// last-rev: 30/01/24

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>
#include <NewKit/Stream.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>

namespace NewOS {
/// @brief Json value class
class JsonType final {
 public:
  explicit JsonType() : NewOS::JsonType(1, 1) {}

  explicit JsonType(SizeT lhsLen, SizeT rhsLen)
      : fKey(lhsLen), fValue(rhsLen) {}

  ~JsonType() = default;

  HCORE_COPY_DEFAULT(JsonType);

 private:
  StringView fKey;
  StringView fValue;

 public:
  /// @brief returns the key of the json
  /// @return the key as string view.
  StringView &AsKey() { return fKey; }

  /// @brief returns the value of the json.
  /// @return the key as string view.
  StringView &AsValue() { return fValue; }

  static JsonType kUndefined;
};

/// @brief Json stream helper class.
struct JsonStreamTrait final {
  JsonType In(const char *full_array) {
    SizeT len = rt_string_len(full_array);

    if (full_array[0] == '\"' && full_array[len - 1] == ',' ||
        full_array[len - 1] == '\"') {
      Boolean probe_key = true;

      SizeT key_len = 0;
      SizeT value_len = 0;

      for (SizeT i = 1; i < len; i++) {
        if (full_array[i] == ' ') continue;

        JsonType type(kPathLen, kPathLen);

        if (probe_key) {
          type.AsKey().Data()[key_len] = full_array[i];
          ++key_len;

          if (full_array[i] == '\"') {
            probe_key = false;
            type.AsKey().Data()[key_len] = 0;

            ++i;
          }
        } else {
          type.AsValue().Data()[value_len] = full_array[i];
          ++value_len;

          if (full_array[i] == '\"') {
            type.AsValue().Data()[value_len] = 0;
          }
        }
      }
    }

    return JsonType::kUndefined;
  }

  JsonType Out(JsonType &out) { return out; }
};

using JsonStream = Stream<JsonStreamTrait, JsonType>;
}  // namespace NewOS
