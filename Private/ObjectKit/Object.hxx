/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

namespace HCore {
/// \brief Object Manager handle.
typedef struct Object {
  Void(*Release)(Void);
  Void(*Invoke)(Void);
  Void(*QueryInterface)(Void);
} Ojbect, *ObjectPtr;
} // namespace HCore

#define object_cast reinterpret_cast
