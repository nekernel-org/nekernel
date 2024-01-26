/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <SystemKit/MePNG.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Utils.hpp>
#include <NewKit/Ref.hpp>

static const char* png_error_to_str(const int err);

enum
{
    kPngErrAdler,
    kPngErrCrc,
    kPngErrCompr,
    kPngUnknown,
};

typedef void* PNGImage;

static const char* png_error_to_str(const int err)
{
    switch (err)
    {
        case kPngErrAdler:
            return "PNG: Bad adler32.";
        case kPngErrCompr:
            return "PngLin: Bad compression.";
        case kPngErrCrc:
            return "PNG: Bad CRC32.";
        case kPngUnknown:
        default:
            return "PNG: Error while loading image.";
    }

    return "PNG: Unknown error.";
}

class PngHeader final
{
public:
    UInt8 Mag1;
    Char* Mag2;
    UInt16 DosDetect;
    UInt16 DosEOF;
    UInt16 UnixLF;

};

#define kIHDRMag "IHDR"
#define kIDATMag "IDAT"
#define kIENDMag "IEND"

MeFilePtr png_open_file(const char* path)
{
    MeFilePtr thePng = new MeFile(path);

    thePng->SetMIME("image/png");
    thePng->Rewind();

    return thePng;
}

static h-core::Ref<PngHeader> png_read_header(MeFilePtr thePng)
{
    if (thePng)
    {
        h-core::Ref<PngHeader> header;

        header.Leak().Mag1 = *(UInt8*)thePng->Read(sizeof(UInt8));

        h-core::rt_copy_memory((h-core::voidPtr)header.Leak().Mag2,
                              (h-core::voidPtr)thePng->Read(h-core::string_length("PNG")),
                                                              h-core::string_length("PNG"));

        header.Leak().DosDetect = *(UInt16*)thePng->Read(sizeof(UInt16));
        header.Leak().DosEOF = *(UInt16*)thePng->Read(sizeof(UInt16));
        header.Leak().UnixLF = *(UInt16*)thePng->Read(sizeof(UInt16));

        return header;
    }

    return h-core::Ref<PngHeader>{ };
}
