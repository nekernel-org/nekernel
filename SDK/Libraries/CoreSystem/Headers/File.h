/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

#define kMaxForkNameLength (256U) /* long fork names. */

struct _Fork;

/// @brief Filesystem wrapper.

typedef QWordType FSRef;

/// @brief Opens a new file.
/// @param path where to find it.
/// @param rest the restrict (rw, rwe, r+, w+, r, w)
/// @return FSRef the file.
CS_EXTERN_C FSRef CSOpenFile(const CharacterTypeUTF8* path, const CharacterTypeUTF8* r);

/// @brief Closes the file and flushes it to the said file.
/// @param refCS the filesystem reference.
/// @return
CS_EXTERN_C VoidType CSCloseFile(FSRef refCS);

/// @brief A fork information header.
typedef struct _Fork
{
	Int32Type		  forkFlags;
	Int32Type		  forkKind;
	CharacterTypeUTF8 forkName[kMaxForkNameLength];
	SizeType		  forkSize;
	CharacterTypeUTF8 forkData[];
} ForkType, ForkTypePtr;

typedef ForkType* FSForkRef;

/// @brief Gets the fork inside a file.
/// @param refCS the filesystem ref
/// @param forkName the fork's name
/// @return the fork data.
CS_EXTERN_C FSForkRef CSGetFork(FSRef refCS, const CharacterTypeUTF8* forkName);

/// @brief Check if the filesystem path is valid.
/// @return if not return false, or true.
CS_EXTERN_C BooleanType CSIsValidPath(const CharacterTypeUTF8* path);

/// END OF FILE
