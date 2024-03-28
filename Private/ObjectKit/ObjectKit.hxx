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
  NewOS::Char ObjectName[255];
  NewOS::Int32 ObjectType;
  NewOS::Char ObjectNamespace[255];

  NewOS::Void(*Release)(struct Object* Self);
  NewOS::IntPtr(*Invoke)(struct Object* Self, NewOS::Int32 Sel, ...);
  NewOS::Void(*Query)(struct Object* Self, NewOS::VoidPtr* Dst, NewOS::SizeT SzDst, NewOS::XRN::GUIDSequence GuidOf);
} *ObjectRef;

#define object_cast reinterpret_cast<ObjectRef>
