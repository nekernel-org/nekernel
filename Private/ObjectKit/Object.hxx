/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <CFKit/GUIDWrapper.hpp>

#define kObjectGlobalNamespaceSystem "HCORE_ROOT\\"
#define kObjectGlobalNamespaceUser "HCORE_USER_ROOT\\"

/// \brief Object handle.
typedef struct Object final {
  HCore::WideChar ObjectName[255];
  HCore::Int32 ObjectType;
  HCore::WideChar ObjectNamespace[255];

  HCore::Void(*Release)(struct Object* Self);
  HCore::IntPtr(*Invoke)(struct Object* Self, HCore::Int32 Sel, ...);
  HCore::Void(*QueryInterface)(HCore::VoidPtr* Dst, HCore::SizeT SzDst, HCore::XRN::GUIDSequence GuidOf);
} Object, *ObjectPtr;

#define object_cast reinterpret_cast
