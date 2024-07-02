/* -------------------------------------------

	Copyright Zeta Electronics Corporation

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
	Kernel::Void (*Release)(struct _Application* Self, Kernel::Int32 ExitCode);
	/// @brief Invoke a function from the application object.
	Kernel::IntPtr (*Invoke)(struct _Application* Self, Kernel::Int32 Sel, ...);
	/// @brief Query a new application object from a GUID.
	/// @note this doesn't query a process, it query a registered object withtin that app.
	Kernel::Void (*Query)(struct _Application* Self, Kernel::VoidPtr* Dst, Kernel::SizeT SzDst, Kernel::XRN::GUIDSequence GuidOf);
} ApplicationInterface, *ApplicationInterfaceRef;

#define app_cast reinterpret_cast<ApplicationInterfaceRef>
