/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <System.Zip/Zip.hpp>
#include <System.Core/Defs.hxx>

#define kInitialSz 4096

namespace System::Zip {
ZipStream::ZipStream()
    : fSharedData(System::Heap::Shared()->New(kInitialSz, kHeapExpandable)),
      fSharedSz(kInitialSz) {}

ZipStream::~ZipStream() noexcept {
  if (fSharedData) System::Heap::Shared()->Delete(fSharedData);
}

FilePtr ZipStream::FlushToFile(const char *name) {
  FilePtr fp = new File(name);
  CA_MUST_PASS(fp);

  this->fSharedSz = System::Heap::Shared()->Size(this->fSharedData);

  fp->MIME(kZipKitMime);
  fp->Write(this->fSharedData, this->fSharedSz);

  return fp;
}

void *ZipStream::Deflate(const char *name) {
  z_stream deflate_stream{0};

  deflate_stream.zalloc = Z_NULL;
  deflate_stream.zfree = Z_NULL;
  deflate_stream.opaque = Z_NULL;

  deflate_stream.avail_in =
      (uInt)this->fSharedSz + 1;  // size of input, string + terminator
  deflate_stream.next_in = (Bytef *)this->fSharedData;   // input char array
  deflate_stream.avail_out = (uInt)this->fSharedSz;      // size of output
  deflate_stream.next_out = (Bytef *)this->fSharedData;  // output char array

  deflateInit(&deflate_stream, 1);
  deflate(&deflate_stream, Z_NO_FLUSH);
  deflateEnd(&deflate_stream);

  return this->fSharedData;
}

void ZipStream::Inflate(const char *name, void *data) {
  z_stream inflate_stream{0};

  inflate_stream.zalloc = Z_NULL;
  inflate_stream.zfree = Z_NULL;
  inflate_stream.opaque = Z_NULL;

  inflate_stream.avail_in =
      (SizeT)((char *)(inflate_stream.next_out -
                       (Bytef *)this->fSharedData));  // size of input

  inflate_stream.next_in = (Bytef *)this->fSharedData;   // input char array
  inflate_stream.avail_out = (uInt)this->fSharedSz;      // size of output
  inflate_stream.next_out = (Bytef *)this->fSharedData;  // output char array

  inflateInit(&inflate_stream);
  inflate(&inflate_stream, Z_NO_FLUSH);
  inflateEnd(&inflate_stream);
}
}  // namespace System.Zip
