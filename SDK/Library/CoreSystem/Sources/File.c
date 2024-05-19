/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Defines.h>
#include <Headers/File.h>

enum FileOp
{
	kFlushFile,
	kReadFork,
	kWriteFork,
	kOpenFork,
	kCloseFork,
};

/// @brief Opens a new file.
/// @param path where to find it.
/// @param rest the restrict (rw, rwe, r+, w+, r, w)
/// @return FSRef the file.
CS_EXTERN_C FSRef FsOpenFile(const CharacterTypeUTF8* path,
							 const CharacterTypeUTF8* rest)
{
	CS_MUST_PASS(kSharedApplication);
	CS_MUST_PASS(path && FsIsValidPath(path) == Yes);
	CS_MUST_PASS(rest);

	return kSharedApplication->Invoke(kSharedApplication, kCallOpenFile, path,
									  rest);
}

/// @brief Closes the file and flushes it to the said file.
/// @param refFs the filesystem reference.
/// @return
CS_EXTERN_C VoidType FsCloseFile(FSRef refFs)
{
	CS_MUST_PASS(kSharedApplication);

	kSharedApplication->Invoke(kSharedApplication, refFs, kFlushFile);
	kSharedApplication->Invoke(kSharedApplication, kCallCloseFile, refFs);
}
