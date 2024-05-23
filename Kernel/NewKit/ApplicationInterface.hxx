/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

///
/// @brief Application object, given by the OS to the process. interact with the OS.
/// @file ApplicationInterface.hxx
/// @author Amlal EL Mahrouss
///

#include <NewKit/Defines.hpp>
#include <CFKit/GUIDWrapper.hpp>

/// \brief Application Interface.
/// \author Amlal El Mahrouss
typedef struct _ApplicationInterface final
{
	/// @brief Releases the object exit the process on main object.
	NewOS::Void (*Release)(struct _Application* Self, NewOS::Int32 ExitCode);
	/// @brief Invoke a function from the application object.
	NewOS::IntPtr (*Invoke)(struct _Application* Self, NewOS::Int32 Sel, ...);
	/// @brief Query a new application object from a GUID.
	/// @note this doesn't query a process, it query a registered object withtin that app.
	NewOS::Void (*Query)(struct _Application* Self, NewOS::VoidPtr* Dst, NewOS::SizeT SzDst, NewOS::XRN::GUIDSequence GuidOf);
} ApplicationInterface, *ApplicationInterfaceRef;

#define app_cast reinterpret_cast<ApplicationInterfaceRef>
