/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <CFKit/GUIDWrapper.hpp>

#define kObjectGlobalNamespaceSystem "HCORE_ROOT\\"
#define kObjectGlobalNamespaceUser "HCORE_USER_ROOT\\"

namespace HCore {
/// \brief Object handle.
typedef struct Object final {
  WideChar ObjectName[255];
  Int32 ObjectType;
  WideChar ObjectNamespace[255];

  Void(*Release)(struct Object* Self);
  Void(*Invoke)(struct Object* Self, Int32 Sel, ...);
  Void(*QueryInterface)(VoidPtr* Dst, SizeT SzDst, XRN::GUIDSequence GuidOf);
} Ojbect, *ObjectPtr;
} // namespace HCore

#define object_cast reinterpret_cast
