/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>
#include <Headers/File.h>

enum FileOp {
    kFlushFile,
    kReadFork,
    kWriteFork,
    kOpenFork,
    kCloseFork,
};

/// @brief Opens a new file.
/// @param path where to find it.
/// @param r the restrict (rw, rwe, r+, w+, r, w)
/// @return 
CA_EXTERN_C FSRef FsOpenFile(const CharacterTypeUTF8* path, const CharacterTypeUTF8* r) {
    CA_MUST_PASS(kSharedApplication);
    CA_MUST_PASS(path);
    CA_MUST_PASS(r);

    return kSharedApplication->Invoke(kSharedApplication, kCallOpenFile, path, r);
}

/// @brief Closes the file and flushes it to the said file.
/// @param refFs the filesystem reference.
/// @return 
CA_EXTERN_C VoidType FsCloseFile(FSRef refFs) {
    CA_MUST_PASS(kSharedApplication);

    kSharedApplication->Invoke(kSharedApplication, refFs, kFlushFile);
    kSharedApplication->Invoke(kSharedApplication, kCallCloseFile, refFs);
}
