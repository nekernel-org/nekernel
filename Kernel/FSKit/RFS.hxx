/* -------------------------------------------

	Copyright ZKA Technologies

	File: RFS.hxx
	Purpose: Resillient File System

	Revision History:

	8/8/2024: Added file (amlel)

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/Defines.hxx>

struct RFS_MASTER_PARTITION_BLOCK;
struct RFS_FILE_PARTITIN_BLOCK;
struct RFS_CATALOG_BLOCK;
struct RFS_FORK_BLOCK;