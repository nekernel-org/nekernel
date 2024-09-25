/* -------------------------------------------

	Copyright ZKA Technologies.

	File: Encrypt.hxx
	Purpose: Encrypted File System

	Revision History:

	8/8/2024: Added file (amlel)

------------------------------------------- */

#pragma once

#include <FSKit/NeFS.hxx>

/**************************************** */
/// @brief Locks an EFS partition.
/**************************************** */

Kernel::Int32 efs_lock_partition(Kernel::Char* partition_name, Kernel::Char* password, Kernel::Size length);

/**************************************** */
/// @brief Unlocks an EFS partition.
/**************************************** */

Kernel::Int32 efs_unlock_partition(Kernel::Char* partition_name, Kernel::Char* password, Kernel::Size length);
