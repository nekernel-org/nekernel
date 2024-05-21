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
CS_EXTERN_C FSRef CSOpenFile(const CharacterTypeUTF8* path,
							 const CharacterTypeUTF8* rest)
{
	CS_MUST_PASS(kSharedApplication);
	CS_MUST_PASS(path && CSIsValidPath(path) == Yes);
	CS_MUST_PASS(rest);

	return kSharedApplication->Invoke(kSharedApplication, kCallOpenFile, path,
									  rest);
}

/// @brief Closes the file and flushes it to the said file.
/// @param refCS the filesystem reference.
/// @return
CS_EXTERN_C VoidType CSCloseFile(FSRef refCS)
{
	CS_MUST_PASS(kSharedApplication);

	kSharedApplication->Invoke(kSharedApplication, refCS, kFlushFile);
	kSharedApplication->Invoke(kSharedApplication, kCallCloseFile, refCS);
}

/// @brief Check if filesystem path is valid.
/// @param path 
/// @return 
CS_EXTERN_C BooleanType CSIsValidPath(const CharacterTypeUTF8* path)
{
	CS_MUST_PASS(kSharedApplication);
	CS_MUST_PASS(path);

	return kSharedApplication->Invoke(kSharedApplication, kCallFileExists, path) ||
			kSharedApplication->Invoke(kSharedApplication, kCallDirectoryExists, path) ||
			kSharedApplication->Invoke(kSharedApplication, kCallSymlinkExists, path) ||
			kSharedApplication->Invoke(kSharedApplication, kCallDriveExists, path) ||
			kSharedApplication->Invoke(kSharedApplication, kCallDeviceExists, path);
}