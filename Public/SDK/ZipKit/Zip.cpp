/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ZipKit/Zip.hpp>

#include <SystemKit/MeHeap.hpp>
#include <SystemKit/MeFile.hpp>

#define kInitialSz 4096

namespace ZipKit
{
    ZipStream::ZipStream()
        : 
        fSharedData(MeHeap::Shared()->New(kInitialSz, kHeapExpandable)),
        fSharedSz(kInitialSz)
    {

    }

    ZipStream::~ZipStream() noexcept
    {
        if (fSharedData)
            MeHeap::Shared()->Dispose(fSharedData);
    }
    
    MeFilePtr ZipStream::FlushToFile(const char* name)
    {
        MeFilePtr fp = new MeFile(name);
        MUST_PASS(fp);

        this->fSharedSz = MeHeap::Shared()->Tell(this->fSharedData);

        fp->SetMIME("application/x-bzip");
        fp->Write(this->fSharedData, this->fSharedSz);
    
        return fp;
    }

    void* ZipStream::Deflate(const char* name)
    {

        return nullptr;
    }

    void ZipStream::Inflate(const char* name, void* data)
    {

    }
}