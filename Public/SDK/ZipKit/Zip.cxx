/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ZipKit/Zip.hpp>

// very hacky thing because of gcc...
#include <SystemKit/CoreAPI.hxx>

#define kInitialSz 4096

namespace ZipKit
{
ZipStream::ZipStream() : fSharedData(MeHeap::Shared()->New(kInitialSz, kHeapExpandable)), fSharedSz(kInitialSz)
{
}

ZipStream::~ZipStream() noexcept
{
    if (fSharedData)
        MeHeap::Shared()->Dispose(fSharedData);
}

MeFilePtr ZipStream::FlushToFile(const char *name)
{
    MeFilePtr fp = new MeFile(name);
    MUST_PASS(fp);

    this->fSharedSz = MeHeap::Shared()->Tell(this->fSharedData);

    fp->SetMIME("application/x-bzip");
    fp->Write(this->fSharedData, this->fSharedSz);

    return fp;
}

void *ZipStream::Deflate(const char *name)
{
    z_stream deflate_stream{0};

    deflate_stream.zalloc = Z_NULL;
    deflate_stream.zfree = Z_NULL;
    deflate_stream.opaque = Z_NULL;

    deflate_stream.avail_in = (uInt)this->fSharedSz + 1;  // size of input, string + terminator
    deflate_stream.next_in = (Bytef *)this->fSharedData;  // input char array
    deflate_stream.avail_out = (uInt)this->fSharedSz;     // size of output
    deflate_stream.next_out = (Bytef *)this->fSharedData; // output char array

    deflateInit(&deflate_stream, 1);
    deflate(&deflate_stream, Z_NO_FLUSH);
    deflateEnd(&deflate_stream);

    return this->fSharedData;
}

void ZipStream::Inflate(const char *name, void *data)
{
    z_stream inflate_stream{0};

    inflate_stream.zalloc = Z_NULL;
    inflate_stream.zfree = Z_NULL;
    inflate_stream.opaque = Z_NULL;

    inflate_stream.avail_in = (SizeT)((char *)(inflate_stream.next_out - (Bytef *)this->fSharedData)); // size of input

    inflate_stream.next_in = (Bytef *)this->fSharedData;  // input char array
    inflate_stream.avail_out = (uInt)this->fSharedSz;     // size of output
    inflate_stream.next_out = (Bytef *)this->fSharedData; // output char array

    inflateInit(&inflate_stream);
    inflate(&inflate_stream, Z_NO_FLUSH);
    inflateEnd(&inflate_stream);
}
} // namespace ZipKit
