/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Zip/Zip.hpp>
#include <System.Core/Defs.hxx>

#define kZipInitialSize 4096

namespace System::Zip {
ZipStream::ZipStream()
    : fSharedData(System::Heap::Shared()->New(kZipInitialSize, kHeapExpandable)),
      fSharedSz(kZipInitialSize) {}

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
  QWORD tmpSharedSz = this->fSharedSz;
  HcUncompress((BYTE*)this->fSharedData, &this->fSharedSz, (BYTE*)this->fSharedData, tmpSharedSz);

  return this->fSharedData;
}

void ZipStream::Inflate(const char *name, BYTE *data, QWORD dataLen) {
  HcCompress((BYTE*)this->fSharedData, &this->fSharedSz, (BYTE*)data, dataLen);
}
}  // namespace System.Zip
