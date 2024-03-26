/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <CFKit/GUIDWrapper.hpp>

#define kObjectGlobalNamespace ":\\"

enum {
  kObjectTypeProcess,
  kObjectTypeFile,
  kObjectTypeDevice,
  kObjectTypeNetwork,
  kObjectTypeInvalid,
  KObjectTypeUserDefined = 0xCF,
  kObjectTypeCount = 5,
};

/// \brief Object handle.
/// \author Amlal El Mahrouss
typedef struct Object final {
  HCore::Char ObjectName[255];
  HCore::Int32 ObjectType;
  HCore::Char ObjectNamespace[255];

  HCore::Void(*Release)(struct Object* Self);
  HCore::IntPtr(*Invoke)(struct Object* Self, HCore::Int32 Sel, ...);
  HCore::Void(*Query)(struct Object* Self, HCore::VoidPtr* Dst, HCore::SizeT SzDst, HCore::XRN::GUIDSequence GuidOf);
} *ObjectRef;

#define object_cast reinterpret_cast<ObjectRef>
