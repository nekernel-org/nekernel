/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

/// @brief Filesystem wrapper.

typedef QWordType FSRef;

/// @brief Opens a new file.
/// @param path where to find it.
/// @param rest the restrict (rw, rwe, r+, w+, r, w)
/// @return FSRef the file.
CA_EXTERN_C FSRef FsOpenFile(const CharacterTypeUTF8* path, const CharacterTypeUTF8* r);

/// @brief Closes the file and flushes it to the said file.
/// @param refFs the filesystem reference.
/// @return
CA_EXTERN_C VoidType FsCloseFile(FSRef refFs);

#define kMaxForkNameLength 256 /* long fork names. */

/// @brief A fork information header.
typedef struct _Fork
{
	PtrVoidType		  forkData;
	SizeType		  forkSize;
	Int32Type		  forkFlags;
	Int32Type		  forkKind;
	CharacterTypeUTF8 forkName[kMaxForkNameLength];
} ForkType;

typedef ForkType* FSForkRef;

/// @brief Gets the fork inside a file.
/// @param refFs the filesystem ref
/// @param forkName the fork's name
/// @return the fork data.
CA_EXTERN_C FSForkRef FsGetFork(FSRef refFs, const CharacterTypeUTF8* forkName);

/// @brief Check if the filesystem path is valid.
/// @return if not return false, or true.
CA_EXTERN_C BooleanType FsIsValidPath(const CharacterTypeUTF8* path);

/// @note not only limited to, there is code forks, icon forks...
#define FsGetDataFork(refFs) FsGetFork(refFs, "data")
#define FsGetRsrcFork(refFs) FsGetFork(refFs, "rsrc")
