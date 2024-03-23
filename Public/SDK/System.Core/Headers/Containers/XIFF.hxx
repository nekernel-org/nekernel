/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __XIFF__
#define __XIFF__

/** ---------------------------------------------------

    * THIS FILE CONTAINS CODE FOR THE eXtended Information File Format.
    * XIFF is used to make setup programs/audio/video files.

------------------------------------------------------- */

#include <System.Core/Headers/Defines.hxx>

/// @brief four-character code for XIFF.
#define kFourCCLength_XIFF 4

#define kXIFFContainerVideo "XVFF"
#define kXIFFContainerAudio "XAFF"
#define kXIFFContainerInstaller "XNFF"
#define kXIFFContainerGeneric "XIFF"
#define kXIFFContainerBinary "XBFF"

/***
 * @brief Generic XIFF header
 * Used by XIFF based containers.
 */

struct PACKED XiffHeader final {
  ByteType f_Magic[kFourCCLength_XIFF];     // XIFF string (includes \0)
  DWordType f_Size;                       // overall size of header (XiffHeader) in bytes
  DWordType f_FormatType;                 // format type. generic
  ByteType f_SpecificMag[kFourCCLength_XIFF];              // The sub header magic
  DWordType f_SpecificSize;               // length of the format data
  DWordType f_SpecificFormatType;         // format type. generic
};

typedef struct XiffHeader XiffHeader;

#endif  // ifndef __XIFF__
