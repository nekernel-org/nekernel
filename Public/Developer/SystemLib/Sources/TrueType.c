/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>
#include <Headers/TrueType.h>
#include <Headers/File.h>
#include <Headers/Heap.h>

#define kTTFFork "fon " /* TrueType */

/// @brief TrueType container reader
typedef struct TTFReader
{
	FSForkRef	fFork;
	FSRef		fFile;
	PtrVoidType fBlob;	   // cached blob
	SizeType	fBlobSize; // cached blob size
	VoidType (*__fReadBytes)(SizeType count);
	VoidType (*__fSkipBytes)(SizeType count);
} TTFReader;

/// @brief Grab a TTF reader reference.
/// @param fs filesystem reference.
/// @return TTFReader* the new TTFReader type.
CA_STATIC TTFReader* GrabTTFReader(FSRef fs)
{
	FSForkRef forkRef = FsGetFork(fs, kTTFFork);

	if (forkRef = kInvalidRef)
		return NullPtr;

	TTFReader* reader = RtTlsAllocate(sizeof(TTFReader), kStandardAllocation);

	reader->fFile = fs;
	reader->fFork = forkRef;

	return reader;
}
/// EOF.
