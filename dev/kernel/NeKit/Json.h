
/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

// last-rev: 02/04/25

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>
#include <NeKit/KString.h>
#include <NeKit/Stream.h>
#include <NeKit/Utils.h>

#define kJSONMaxLen (8196)
#define kJSONLen (256)
#define kJSONNullArr "[]"
#define kJSONNullObj "{}"

namespace Kernel {
/// @brief JavaScript object class
class Json final {
 public:
  explicit Json() {
    auto    len = kJSONMaxLen;
    BasicKString<> key = KString(len);
    key += kJSONNullObj;

    this->AsKey()   = key;
    this->AsValue() = key;
  }

  explicit Json(SizeT lhsLen, SizeT rhsLen) : fKey(lhsLen), fValue(rhsLen) {}

  ~Json() = default;

  NE_COPY_DEFAULT(Json)

  Bool& IsUndefined() { return fUndefined; }

 private:
  Bool    fUndefined;  // is this instance undefined?
  BasicKString<> fKey;
  BasicKString<> fValue;

 public:
  /// @brief returns the key of the json
  /// @return the key as string view.
  BasicKString<>& AsKey() { return fKey; }

  /// @brief returns the value of the json.
  /// @return the key as string view.
  BasicKString<>& AsValue() { return fValue; }

  static Json kNull;
};

/// @brief Json stream reader helper.
struct JsonStreamReader final {
  STATIC Json In(const Char* full_array) {
    auto    start_val   = '{';
    auto    end_val     = '}';
    Boolean probe_value = false;

    if (full_array[0] != start_val) {
      if (full_array[0] != '[') return Json::kNull;

      start_val = '[';
      end_val   = ']';

      probe_value = true;
    }

    SizeT len = rt_string_len(full_array);

    SizeT key_len   = 0;
    SizeT value_len = 0;

    Json type(kJSONMaxLen, kJSONMaxLen);

    for (SizeT i = 1; i < len; ++i) {
      if (full_array[i] == '\r' || full_array[i] == '\n') continue;

      if (probe_value) {
        if (full_array[i] == end_val || full_array[i] == ',') {
          probe_value = false;

          ++value_len;
        } else {
          if (full_array[i] == '\'') {
            type.AsValue().Data()[value_len] = 0;
            break;
          }

          type.AsValue().Data()[value_len] = full_array[i];

          ++value_len;
        }
      } else {
        if (start_val == '[') continue;

        if (full_array[i] == ':') {
          type.AsKey().Data()[key_len] = 0;
          ++key_len;

          ++i;

          while (full_array[i] == ' ' || full_array[i] == '\t') ++i;

          probe_value = true;
        } else {
          type.AsKey().Data()[key_len] = full_array[i];

          ++key_len;
        }
      }
    }

    type.AsValue().Data()[value_len] = 0;

    return type;
  }
};

using JsonStream = Stream<JsonStreamReader, Json>;
}  // namespace Kernel
