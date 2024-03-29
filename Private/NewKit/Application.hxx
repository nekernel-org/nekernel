/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

///
/// @brief Main application object, given by the OS to interact with the OS.
/// @file Application.hxx
/// @author Amlal EL Mahrouss
///

#include <NewKit/Defines.hpp>
#include <CFKit/GUIDWrapper.hpp>

/// \brief Application Interface.
/// \author Amlal El Mahrouss
typedef struct Application final {
  /// @brief Releases the object exit the process on main object.
  NewOS::Void(*Release)(struct Application* Self, NewOS::Int32 ExitCode);
  /// @brief Invoke a function from the application object.
  NewOS::IntPtr(*Invoke)(struct Application* Self, NewOS::Int32 Sel, ...);
  /// @brief Query a new application object from a GUID.
  /// @note this doesn't query a process, it query a registered object withtin that app.
  NewOS::Void(*Query)(struct Application* Self, NewOS::VoidPtr* Dst, NewOS::SizeT SzDst, NewOS::XRN::GUIDSequence GuidOf);
} Application, *ApplicationRef;

#define object_cast reinterpret_cast<ApplicationRef>
