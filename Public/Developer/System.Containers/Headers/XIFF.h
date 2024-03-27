/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __XIFF__
#define __XIFF__

/** ---------------------------------------------------

    * The eXtended Information File Format.
    * XIFF is used to make setup programs/audio/video files.

------------------------------------------------------- */

#include <System.Core/Headers/Defines.h>

/// @brief four-character code for XIFF.
#define kFourCCLength_XIFF 4
#define kXIFFNameLength 255

#define kXIFFContainerVideo "XVFF"
#define kXIFFContainerAudio "XAFF"
#define kXIFFContainerInstaller "XNFF"
#define kXIFFContainerGeneric "XIFF"
#define kXIFFContainerBinary "XBFF"

/***
 * @brief Generic XIFF header
 * Used by XIFF based containers.
 */

struct PACKED XiffHeader {
  ByteType f_Magic[kFourCCLength_XIFF];     // XIFF string (includes \0)
  DWordType f_Size;                       // overall size of header (XiffHeader) in bytes
  DWordType f_FormatType;                 // format type. generic
  ByteType f_SpecificMag[kFourCCLength_XIFF];              // The sub header magic
  DWordType f_SpecificSize;               // length of the format data
  DWordType f_SpecificFormatType;         // format type. generic
};

typedef struct XiffHeader XiffHeader;

/// @brief XIFF metadata header, either located in forks or in file directly.
/// @author Amlal EL Mahrouss
struct PACKED XiffMetadataHeader {
  ByteType f_Name[kXIFFNameLength];
  DWordType f_Flags;
  DWordType f_Type;
  QWordType f_Offset;
  SizeType f_Size;
};

/// Pascal string
#define kXIFFStringMetadata4CC    "strp"

/// TrueType font
#define kXIFFFontMetadata4CC      "font"

/// UI resource
#define kXIFFResourceMetadata4CC  "resx"

#endif  // ifndef __XIFF__
