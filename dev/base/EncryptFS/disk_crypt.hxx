/* -------------------------------------------

	Copyright ZKA Web Services Co.

	File: disk_encrypt.hxx
	Purpose: Disk Encryption File System

	Revision History:

	8/8/2024: Added file (amlel)

------------------------------------------- */

#pragma once

#include <FSKit/NeFS.hxx>

namespace Kernel
{
	/**************************************** */
	/// @brief Locks an EFS partition.
	/**************************************** */

	Int32 efs_lock_partition(const Char* partition_name, const Char* password, SizeT length);

	/**************************************** */
	/// @brief Unlocks an EFS partition.
	/**************************************** */

	Int32 efs_unlock_partition(const Char* partition_name, const Char* password, SizeT length);
} // namespace Kernel
