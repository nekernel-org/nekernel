/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

/// @brief Filesystem wrapper.

typedef QWordType FSRef;

/// @brief Opens a new file.
/// @param path where to find it.
/// @param r the restrict (rw, rwe, r+, w+, r, w)
/// @return 
CA_EXTERN_C FSRef FsOpenFile(const CharacterTypeUTF8* path, const CharacterTypeUTF8* r);

/// @brief Closes the file and flushes it to the said file.
/// @param refFs the filesystem reference.
/// @return 
CA_EXTERN_C VoidType FsCloseFile(FSRef refFs);

typedef QWordType FSForkRef;

/// @brief A fork information header.
typedef struct _Fork {
    PtrVoidType forkData;
    SizeType forkSize;
    Int32Type forkFlags;
    Int32Type forkKind;
    CharacterTypeUTF8 forkName[256];
} ForkType;

/// @brief Gets the fork inside a file.
/// @param refFs the filesystem ref
/// @param forkName the fork's name
/// @return the fork data.
CA_EXTERN_C FSForkRef FsGetFork(FSRef refFs, const CharacterTypeUTF8* forkName);

#define FsGetDataFork(refFs) FsGetFork(refFs, "data")
#define FsGetRsrcFork(refFs) FsGetFork(refFs, "rsrc")
